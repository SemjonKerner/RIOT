/*
 * Copyright (C) 2018-2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_gnrc_netif_6top
 * @{
 *
 * @file
 * @brief       GNRC 6top netif and openwsn mac interface
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 *
 * @}
 */

// includes -------------------------------------------------------------------
#include <stdio.h>

#include "fmt.h"

#include "od.h"
#include "openwsn.h"
#include "net/ipv6/addr.h"

#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/nettype.h"

#include "opendefs.h"
#include "errno.h"
#include "02a-MAClow/IEEE802154E.h"
#include "02b-MAChigh/sixtop.h"
#include "02b-MAChigh/schedule.h"
#include "03b-IPv6/icmpv6rpl.h"
#include "04-TRAN/openudp.h"
#include "inc/opendefs.h"
#include "cross-layers/openqueue.h"
#include "cross-layers/idmanager.h"
#include "cross-layers/packetfunctions.h"

// defines --------------------------------------------------------------------
#ifdef MODULE_GNRC_SIXLOWPAN
#define NETTYPE                 GNRC_NETTYPE_SIXLOWPAN
#elif defined(MODULE_GNRC_IPV6)
#define NETTYPE                 GNRC_NETTYPE_IPV6
#else
#define NETTYPE                 GNRC_NETTYPE_UNDEF
#endif

// static variables -----------------------------------------------------------
static gnrc_netif_t _netif_6top;
static gnrc_nettype_t _nettype = NETTYPE;
static open_addr_t *_addr = NULL; //TODO _addr is openwsn specific
static char _stack[THREAD_STACKSIZE_DEFAULT]; /* allocate stack for netif */

// netif functions ------------------------------------------------------------
static void _netif_init(gnrc_netif_t *netif)
{

    gnrc_netif_default_init(netif);

#ifdef ENABLE_DEBUG
    printf("[6top] init addr: ");
    for (int i = 0; i < 8; i++) { printf("%02x", addr->addr_64b[i]); }
    printf("\n");
#endif

// set this define on commandline to have a master
#ifdef PANCOORD
    puts("PANCOORD");
    openwsn_mlme_set_role(OW_ROLE_PAN_COORDINATOR);
    openwsn_mlme_set_slotframe_request(101);
    openwsn_mlme_set_link_request(true /*add*/, 0 /*slot*/, \
                OW_CELL_ADV/*type*/, true /*ALOHA*/, 0 /*channel*/, \
                0 /*addr*/);
#else
    puts("LEAF");
    openwsn_mlme_set_role(OW_ROLE_LEAF);
#endif
}

static int _netif_send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    (void)netif;
    int res;

    gnrc_pktsnip_t *tmp = pkt;
    gnrc_netif_hdr_t *hdr = (gnrc_netif_hdr_t *)pkt->data;
    while (true) {
        /*printf("size: %i/ntype: %d/n,pkt", \
               (unsigned int)tmp->size, (int)tmp->type);*/
        if (tmp->next == NULL) {
            break;
        }
        tmp = tmp->next;
    }

    //TODO: format pkt to addr and data - use gnrc functions
    char *data = pkt->data;
    size_t len = pkt->size;

    //res = send byte
    if (hdr->flags & (GNRC_NETIF_HDR_FLAGS_BROADCAST | \
                      GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        openwsn_mcps_data_request(NULL, data, len);
    }
    /*TODO else {
        openwsn_mcps_data_request(addr, data, len);
    }*/
    res = (int)gnrc_pkt_len(pkt->next);

    gnrc_pktbuf_release(pkt);
    return res;
}

/* not used, we pass incoming data to GNRC from openwsn thread in _on_data()*/
static gnrc_pktsnip_t *_netif_recv(gnrc_netif_t *netif)
{
    (void)netif;
    return NULL;
}

static const gnrc_netif_ops_t _netif_6top_ops = {
    .init = _netif_init,
    .send = _netif_send,
    .recv = _netif_recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
    .msg_handler = NULL,
};

// netdev function wrappers ---------------------------------------------------
static inline int _netdev_init(netdev_t *dev)
{
    _addr = idmanager_getMyID(ADDR_64B);
    //_netif_6top = dev->context; TODO WHATZEFUG
    (void)dev;

    // get mac address from openwsn
    memcpy(_netif_6top.l2addr, _addr->addr_64b, sizeof(uint8_t[8]));

    return 0;
}

static inline int _netdev_get(netdev_t *dev, netopt_t opt, void *value,
                              size_t max_len)
{
    (void)dev;
    (void)value;
    (void)max_len;
    int res = -ENOTSUP;

    switch (opt) {
        case NETOPT_ADDRESS: //TODO _addr is openwsn specific
            memcpy(value, _addr->addr_16b, IEEE802154_SHORT_ADDRESS_LEN);
            res = IEEE802154_SHORT_ADDRESS_LEN;
            break;
        case NETOPT_ADDRESS_LONG:
            assert(max_len >= IEEE802154_LONG_ADDRESS_LEN);
            memcpy(value, _netif_6top.l2addr, IEEE802154_LONG_ADDRESS_LEN);
            res = IEEE802154_LONG_ADDRESS_LEN;
            break;
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)value) = IEEE802154_LONG_ADDRESS_LEN;
            res = sizeof(uint16_t);
            break;
        case NETOPT_MAX_PDU_SIZE:
            assert(max_len >= sizeof(uint16_t));
            *((uint16_t *)value) = (IEEE802154_FRAME_LEN_MAX);
            res = sizeof(uint16_t);
            break;
        case NETOPT_PROTO:
            assert(max_len == sizeof(_nettype));
            *((gnrc_nettype_t *)value) = _nettype;
            res = sizeof(gnrc_nettype_t);
            break;
        case NETOPT_DEVICE_TYPE:
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)value) = NETDEV_TYPE_IEEE802154;
            res = sizeof(uint16_t);
            break;
        default:
            break;
    }

    return res;
}

static inline int _netdev_set(netdev_t *dev, netopt_t opt, \
                              const void *value, size_t val_len)
{
    (void)dev;
    (void)value;
    (void)val_len;
    int res = -ENOTSUP;

    switch (opt) {
        case NETOPT_PROTO:
            assert(val_len == sizeof(_nettype));
            memcpy(&_nettype, value, sizeof(_nettype));
            res = sizeof(_nettype);
            break;
        default:
            break;
    }

    return res;
}

static const netdev_driver_t _netdev_6top_driver = {
    .send = NULL,
    .recv = NULL,
    .init = _netdev_init,
    .isr  =  NULL,
    .get  = _netdev_get,
    .set  = _netdev_set,
};

static netdev_t _netdev_6top_dummy = {
    .driver = &_netdev_6top_driver,
};

// openwsn management functions -----------------------------------------------
void ow_mcps_data_confirm(void)
{
    //TODO packet receive notification to gnrc
    printf("RCV");
}

void ow_mcps_data_indication(char *data, size_t data_len)
{
    //TODO
    //read msg
    printf("Received message: ");
    for(unsigned i=0;i<data_len;i++) {
        printf("%c", data[i]);
    }
    puts("");
    //switch header

    //pass to gnrc

}

void mlme_sync_indication(void)
{
    //TODO: RX/TX ready
    printf("SYNC");
}

void mlme_sync_loss_indication(void)
{
    //TODO: RX/TX not possible
    printf("DESYNC");
}

void gnrc_6top_init(void)
{
    puts("INIT 6TOP");

    gnrc_netif_create(&_netif_6top, _stack, sizeof(_stack), GNRC_NETIF_PRIO, "gnrc_6top", \
                      &_netdev_6top_dummy, &_netif_6top_ops);
}

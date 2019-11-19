/*
 * Copyright (C) 2018-2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     gnrc_netif_6top
 * @{
 *
 * @file
 * @brief       GNRC netif implementation for 6top
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 *
 * @}
 */

#include <limits.h>
#include <errno.h>

#include "assert.h"
#include "thread.h"
#include "thread_flags.h"

#include "net/ble.h"
#include "net/bluetil/addr.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/nettype.h"

#define ENABLE_DEBUG            (0)
#include "debug.h"

#ifdef MODULE_GNRC_SIXLOWPAN
#define NETTYPE                 GNRC_NETTYPE_SIXLOWPAN
#elif defined(MODULE_GNRC_IPV6)
#define NETTYPE                 GNRC_NETTYPE_IPV6
#else
#define NETTYPE                 GNRC_NETTYPE_UNDEF
#endif

/* keep the actual device state */
static gnrc_nettype_t _nettype = NETTYPE;

static void _netif_init(gnrc_netif_t *netif)
{
    (void)netif;

    gnrc_netif_default_init(netif);
    /* save the threads context pointer, so we can set its flags */
    _netif_thread = (thread_t *)thread_get(thread_getpid());
}

static int _netif_send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    assert(pkt->type == GNRC_NETTYPE_NETIF);

    (void)netif;
    int res;

    gnrc_netif_hdr_t *hdr = (gnrc_netif_hdr_t *)pkt->data;
    /* if packet is bcast or mcast, we send without addr */
    if (hdr->flags &
        (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
    }
    /* send unicast */
    else {
        res = _send_pkt(pkt->next);
    }

    /* release the packet in GNRC's packet buffer */
    gnrc_pktbuf_release(pkt);
    return res;
}

/* not used, we pass incoming data to GNRC directly from the NimBLE thread */
static gnrc_pktsnip_t *_netif_recv(gnrc_netif_t *netif)
{
    (void)netif;
    /*wenn openwsn einen thread nutzt, dann "on_data" function als openwsn cb registrieren und
     *  = 15.4 header parsen*/
    /*TODO: copy pasta von ieee802.15.4*/
    return NULL;
}

static const gnrc_netif_ops_t _6top_netif_ops = {
    .init = _netif_init,
    .send = _netif_send,
    .recv = _netif_recv,
    .get =  NULL, // gnrc_netif_get_from_netdev
    .set = NULL, // gnrc_netif_set_from_netdev
    .msg_handler = NULL,
};

static void _recv_cb_dummy()
{
    /* allocate netif header */
    gnrc_pktsnip_t *if_snip = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    if (if_snip != NULL) {
        gnrc_netif_hdr_t *netif_hdr = (gnrc_netif_hdr_t *)if_snip->data;
        netif_hdr->if_pid = _gnrc_6top_netif->pid;

        gnrc_netif_hdr_set_netif(if_snip->data, _gnrc_6top_netif);

        /* allocate space in the pktbuf to store the packet */
        uint8_t payload_data[] = {/*TODO 15.4 header */ 0x7A, 0x33, 0x3B};

        /* copy payload from mbuf into pktbuffer */
        gnrc_pktsnip_t *payload = gnrc_pktbuf_add(if_snip, payload_data,
                                        sizeof(payload_data), _nettype);
        gnrc_pktbuf_release(payload);

        /* finally dispatch the receive packet to GNRC */
        if (!gnrc_netapi_dispatch_receive(payload->type,
                        GNRC_NETREG_DEMUX_CTX_ALL, payload)) {
            gnrc_pktbuf_release(payload);
        }
    }
}

gnrc_netif_t *gnrc_6top_create(char *stack, int stacksize,
                                           char priority, char *name,
                                           netdev_t *netdev)
{
    return gnrc_netif_create(stack, stacksize, priority, name, netdev,
                             &_6top_netif_ops);
}


/*
openwsn_bootstrap() - init function : auto_init
openwsn_mcps_data_request(addr, buffer, bufferlen) - send function
openwsn_mlme_set_slotframe_request(slotframelen) - set slotframe len
res = openwsn_mlme_set_link_request(bool add/rm, channel, rx/tx/adv,
                                    bool ???, slt_ofst, addr) - init cell
openwsn_mlme_set_role(role) - set role (pancoord, coord, leaf)
addr = idmanager_getMyID(ADDR_64B) - get mac addr of node

mlme_sync_indication - callback when synced
mlme_sync_loss_indication - callback when desynced
ow_mcps_data_confirm(status) - callback when send, status != 0 is an error
ow_mcps_data_indication(char *data, datalen) - recv function
*/

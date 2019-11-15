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

/* allocate a stack for the netif device */
static char _stack[THREAD_STACKSIZE_DEFAULT];
static thread_t *_netif_thread;

/* keep the actual device state */
static gnrc_netif_t* _gnrc_6top_netif = NULL;
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
    /* if packet is bcast or mcast, we send it to every connected node */
    if (hdr->flags &
        (GNRC_NETIF_HDR_FLAGS_BROADCAST | GNRC_NETIF_HDR_FLAGS_MULTICAST)) {
        nimble_netif_conn_foreach(NIMBLE_NETIF_L2CAP_CONNECTED,
                                  _netif_send_iter, pkt->next);
        res = (int)gnrc_pkt_len(pkt->next);
    }
    /* send unicast */
    else {
        int handle = nimble_netif_conn_get_by_addr(
            gnrc_netif_hdr_get_dst_addr(hdr));
        nimble_netif_conn_t *conn = nimble_netif_conn_get(handle);
        res = _send_pkt(conn, pkt->next);
    }

    /* release the packet in GNRC's packet buffer */
    gnrc_pktbuf_release(pkt);
    return res;
}

/* not used, we pass incoming data to GNRC directly from the NimBLE thread */
static gnrc_pktsnip_t *_netif_recv(gnrc_netif_t *netif)
{
    (void)netif;
    return NULL;
}

static const gnrc_netif_ops_t _nimble_netif_ops = {
    .init = _netif_init,
    .send = _netif_send,
    .recv = _netif_recv,
    .get =  NULL, // gnrc_netif_get_from_netdev,
    .set = NULL, // gnrc_netif_set_from_netdev,
    .msg_handler = NULL,
};

/* awaits netdev_t with an initialized netdev_driver_t */
static netdev_t* _6top_netdev;
void gnrc_6top_netdev_init(netdev_driver_t* gnrc_6top_netdev)
{
    _6top_netdev = gnrc_6top_netdev;
}

static void _recv_dummy()
{
    /* allocate netif header */
    gnrc_pktsnip_t *if_snip = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    if (if_snip != NULL) {
        gnrc_netif_hdr_t *netif_hdr = (gnrc_netif_hdr_t *)if_snip->data;
        netif_hdr->if_pid = _gnrc_6top_netif->pid;

        gnrc_netif_hdr_set_netif(if_snip->data, _gnrc_6top_netif);

        /* allocate space in the pktbuf to store the packet */
        uint8_t payload_data[] = {0x7A, 0x33, 0x3B};

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

void gnrc_6top_init(void)
{
    int res;
    (void)res;

    /* setup the connection context table */
    nimble_netif_conn_init();

    /* initialize of BLE related buffers */
    res = os_mempool_init(&_mem_pool, MBUF_CNT, MBUF_SIZE, _mem, "nim_gnrc");
    assert(res == 0);
    res = os_mbuf_pool_init(&_mbuf_pool, &_mem_pool, MBUF_SIZE, MBUF_CNT);
    assert(res == 0);

    res = ble_l2cap_create_server(NIMBLE_NETIF_CID, MTU_SIZE,
                                  _on_l2cap_server_evt, NULL);
    assert(res == 0);
    (void)res;

    gnrc_netif_create(_stack, sizeof(_stack), GNRC_NETIF_PRIO,
                      "nimble_netif", &_nimble_netdev_dummy, &_nimble_netif_ops);
}

/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup examples
 * @{
 *
 * @file        sensor
 * @brief       gets distance value and sends them over gnrc
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "xtimer.h"
#include "timex.h"
#include "srf04.h"
#include "srf04_params.h"
#include "net/gnrc.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netapi.h"
#include "net/netdev.h"

#define SAMPLE_PERIOD       (50U * US_PER_MS)
#define COMM_MSG_LEN        sizeof(unsigned int)
#define CONF_COMM_RCV_ADDR  {0x23, 0x01}
#define CONF_COMM_PAN       (0xf0ba)
#define CONF_COMM_CHAN      (11U)
#define CONF_COMM_ADDR      {0x22, 0x33}

static kernel_pid_t if_pid;

static void _send_distance(unsigned int distance)
{
    printf("SEND d: %d\n", distance);
    uint8_t ctrl[COMM_MSG_LEN];
    uint8_t buzzer[2] = CONF_COMM_RCV_ADDR;
    gnrc_pktsnip_t *pkt;
    gnrc_netif_hdr_t *nethdr;

    memcpy(&(ctrl[0]), &distance, COMM_MSG_LEN);

    pkt = gnrc_pktbuf_add(NULL, ctrl, COMM_MSG_LEN, GNRC_NETTYPE_UNDEF);
    pkt = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_netif_hdr_t) + 2, GNRC_NETTYPE_NETIF);
    nethdr = (gnrc_netif_hdr_t *)pkt->data;
    gnrc_netif_hdr_init(nethdr, 0, 2);
    gnrc_netif_hdr_set_dst_addr(nethdr, buzzer, 2);
    gnrc_netapi_send(if_pid, pkt);
}

int main(void)
{
    puts("Lange Nacht der Wissenschaft 2018");
    puts("SENSOR");

    /* initialize sensor */
    srf04_t dev;
    if (srf04_init(&dev, &srf04_params[0]) != SRF04_OK) {
        puts("Error: initializing");
        return 1;
    }
    unsigned int distance = 0;

    /* initialize gnrc network */
    gnrc_netif_t *netif = gnrc_netif_iter(NULL);
    if(netif == NULL) {
        puts("comm: ERROR during init, radio not found\n");
        return -1;
    }

    if_pid = netif->pid;

    uint16_t pan = CONF_COMM_PAN;
    gnrc_netapi_set(if_pid, NETOPT_NID, 0, &pan, 2);

    uint16_t chan = CONF_COMM_CHAN;
    gnrc_netapi_set(if_pid, NETOPT_CHANNEL, 0, &chan, 2);

    uint8_t addr[2] = CONF_COMM_ADDR;
    gnrc_netapi_set(if_pid, NETOPT_ADDRESS, 0, &addr, 2);


    /* loop */
    while (1) {
        /* get distance data*/
        distance = srf04_get_distance(&dev);

        /* send data over gnrc */
        if (distance > 0)
            _send_distance(distance);
        else
            continue;

        /* wait a little */
        xtimer_usleep(100000);
    }

    /* never reached */
    return 0;
}

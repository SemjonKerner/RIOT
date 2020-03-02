/*
 * Copyright (C) 2014-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup examples
 * @{
 *
 * @file        buzzer
 * @brief       receives distance and controls buzzer
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
#include "net/gnrc.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netapi.h"
#include "net/netdev.h"
#include "periph/pwm.h"

#define SAMPLE_COUNT        (5U)
#define CONF_COMM_PAN       (0xf0ba)
#define CONF_COMM_BUZZER    {0x23, 0x01}
#define CONF_COMM_CHAN      (11U)
#define BUZZER_QUEUE_SIZE   (8)
#define LOUDNESS            (80U)

static msg_t buzzer_msg_queue[BUZZER_QUEUE_SIZE];

int res[12] =   { 965, 902, 853, 804, 759, 716, 676, 638, 602, 568, 536, 506};
int notes[60] = { 130, 139, 147, 155, 165, 175, 185, 196, 208, 220, 233, 247,
                  260, 278, 294, 310, 330, 350, 370, 392, 416, 440, 466, 494,
                  520, 556, 588, 620, 660, 700, 740, 784, 832, 880, 932, 988,
                 1040,1112,1176,1240,1320,1400,1480,1568,1664,1760,1864,1976,
                 2080,2224,2352,2480,2640,2800,2960,3136,3328,3520,3728,3952};

int main(void)
{
    puts("Lange Nacht der Wissenschaften 2018\n");
    puts("BUZZER");

    /* init pwm */
    uint32_t pwm_freq;
    unsigned int distance = 0;

    /* init gnrc network */
    uint16_t pan = CONF_COMM_PAN;
    uint16_t chan = CONF_COMM_CHAN;
    uint8_t buzzer[2] = CONF_COMM_BUZZER;
    uint8_t *data;

    gnrc_netif_t *netif = gnrc_netif_iter(NULL);
    if(netif == NULL) {
        puts("comm: ERROR during init, radio not found\n");
        return -1;
    }

    puts("comm: setting address and PAN");
    gnrc_netapi_set(netif->pid, NETOPT_NID, 0, &pan, 2);
    gnrc_netapi_set(netif->pid, NETOPT_CHANNEL, 0, &chan, 2);
    gnrc_netapi_set(netif->pid, NETOPT_ADDRESS, 0, &buzzer, 2);
    xtimer_sleep(2);
    puts("RECEIVE");

    gnrc_netreg_entry_t netreg;
    gnrc_pktsnip_t *snip;
    msg_t msg;

    msg_init_queue(buzzer_msg_queue, BUZZER_QUEUE_SIZE);

    netreg.target.pid = thread_getpid();
    netreg.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &netreg);

    for (int i = 0; i < 12; i+=2) {
        pwm_init(PWM_DEV(0), PWM_LEFT, notes[i], res[i]);
        pwm_set(PWM_DEV(0), 0, (LOUDNESS % 100));
        xtimer_usleep(100000);
    }
    pwm_set(PWM_DEV(0), 0 , 0);


    /* loop */
    while (1) {
        /* receive distance data */
        msg_receive(&msg);

        if (msg.type == GNRC_NETAPI_MSG_TYPE_RCV) {
            snip = (gnrc_pktsnip_t *)msg.content.ptr;
            data = snip->data;
            memcpy(&distance, &(data[0]), sizeof(unsigned int));
            printf("RECEIVE d: %d\n", distance);
            gnrc_pktbuf_release(snip);
        }

        /* parse data to pwm */
        if ((distance < 100) | (distance > 1300)) {
            pwm_set(PWM_DEV(0), 0 , 0);
            continue;
        }

        /* normalize distance value to frequency */
        distance -= 100;
        distance /= 20;

        pwm_freq = pwm_init(PWM_DEV(0), PWM_LEFT,
                            notes[distance], res[distance % 12]);
        if (pwm_freq == 0) {
            pwm_set(PWM_DEV(0), 0 , 0);
            continue;
        }

        pwm_set(PWM_DEV(0), 0, (LOUDNESS % 100));

        /* wait a little and stop pwm */
        xtimer_usleep(100000);
        pwm_set(PWM_DEV(0), 0 , 0);

    }
    /* never reached */
    return 0;

}




/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 *
 * @author      Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012
 * @author      Tengfei Chang <tengfei.chang@gmail.com>, July 2012
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, July 2017
 *
 * @}
 */

#ifndef __BOARD_INFO_H
#define __BOARD_INFO_H

#include "stdint.h"
#include "string.h"
#include "cpu.h"

//=========================== defines =========================================

#define INTERRUPT_DECLARATION()
#define DISABLE_INTERRUPTS()                irq_disable();
#define ENABLE_INTERRUPTS()                 irq_enable();

//===== timer

#define PORT_TIMER_WIDTH                    uint32_t
#define PORT_RADIOTIMER_WIDTH               uint32_t

#define PORT_SIGNED_INT_WIDTH               int32_t
#define PORT_TICS_PER_MS                    33
#define SCHEDULER_WAKEUP()
#define SCHEDULER_ENABLE_INTERRUPT()

// TODO: CHECK TIMINGS!
//===== IEEE802154E timing

#define SLOTDURATION                        20    // in miliseconds

// time-slot related
#define PORT_TsSlotDuration                 655   //    20ms

// execution speed related
#define PORT_maxTxDataPrepare               110   //  3355us (not measured)
#define PORT_maxRxAckPrepare                20    //   610us (not measured)
#define PORT_maxRxDataPrepare               33    //  1000us (not measured)
#define PORT_maxTxAckPrepare                50    //  1525us (not measured)

// radio speed related
#define PORT_delayTx                        18    //   549us (not measured)
#define PORT_delayRx                         0    //     0us (can not measure)

//===== adaptive_sync accuracy

#define SYNC_ACCURACY                        2    // ticks

//=========================== variables =======================================

static const uint8_t rreg_uriquery[] = "h=ucb";
static const uint8_t infoBoardname[] = "riot-os";
static const uint8_t infouCName[] = "various";
static const uint8_t infoRadioName[] = "riot-netdev";

#endif

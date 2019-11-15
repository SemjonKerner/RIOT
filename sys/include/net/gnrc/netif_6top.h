/*
 * Copyright (C) 2018-2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_6top GNRC netif Implementation
 * @ingroup     net_gnrc
 * @brief       GNRC netif implementation for gnrc 6top
 *
 * @{
 *
 * @file
 * @brief       GNRC netif implementation for gnrc 6top
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 */

#ifndef NET_GNRC_NETIF_6TOP_H
#define NET_GNRC_NETIF_6TOP_H

#include <stdint.h>

#include "net/gnrc/netif_6top.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the netif implementation, spawns the netif thread
 *
 * This function is meant to be called once during system initialization, i.e.
 * auto-init.
 */
void gnrc_6top_init(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_6TOP_H */
/** @} */

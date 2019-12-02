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
 * # About
 * This NimBLE submodule provides a GNRC netif wrapper for integrating NimBLE
 * with GNRC and other network stacks using netif (e.g. CCNlite).
 *
 * # Concept
 * According to the IPv6-over-BLE standards (RFC7668 and IPSP), this module
 * exposes a (configurable) number of point-to-point BLE connections as a single
 * network device to BLE. Unicast traffic is only send using the corresponding
 * BLE connection. Multicast and Broadcast packets are duplicated and send via
 * each open BLE connection.
 *
 * # Structure
 * The netif implementation is able to handle multiple connections
 * simultaneously. The maximum number of concurrent connections is configured
 * during compile time, using NimBLEs MYNEWT_VAL_BLE_MAX_CONNECTIONS option.
 * Dependent on this value, the netif implementation takes care of allocation
 * all the memory needed. The API of this submodule uses simply integer values
 * to reference the used connection context (like file descriptors in linux).
 *
 * Like any other GNRC network device, the NimBLE netif wrapper runs in its own
 * thread. This thread is started and configured by the common netif code. All
 * send and get/set operations are handled by this thread. For efficiency
 * reasons, receiving of data is however handled completely in the NimBLE host
 * thread, from where the received data is directly passed on to the
 * corresponding GNRC thread.
 *
 * Although the wrapper hooks into GNRC using the netif interface, it does need
 * to implement parts of the netdev interface as well. This is done where
 * needed.
 *
 * # Usage
 * This submodule is designed to work fully asynchronous, in the same way as the
 * NimBLE interfaces are designed. All functions in this submodule will only
 * trigger the intended action. Once this action is complete, the module will
 * report the result asynchronously using the configured callback.
 *
 * So before using this module, make sure to register a callback using the
 * @ref nimble_netif_eventcb() function.
 *
 * After this, this module provides functions for managing BLE connections to
 * other devices. Once these connections are established, this module takes care
 * of mapping IP packets to the corresponding connections.
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
 * @brief Init 6top network interface that is on top of openwsn
 */
void gnrc_6top_init(void);


/**
 * TODO
 */
void ow_mcps_data_confirm(void);
void ow_mcps_data_indication(char *data, size_t data_len);
void mlme_sync_indication(void);
void mlme_sync_loss_indication(void);
void gnrc_6top_init(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_6TOP_H */
/** @} */

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
 * @brief       RIOT adaption definition of the "eui64" bsp module
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include "string.h"
#include "eui64.h"
#include "radio_ow.h"

#include "luid.h"
#include "net/netdev.h"
#include "net/netopt.h"
#include "net/ieee802154.h"

extern radio_vars_t radio_vars;

void eui64_get(uint8_t *addressToWrite)
{
    eui64_t eui64;

    if (radio_vars.dev->driver->get(radio_vars.dev, NETOPT_ADDRESS_LONG, &eui64,
        sizeof(eui64_t)) == sizeof(eui64_t)) {
        memcpy(addressToWrite, eui64.uint8, sizeof(eui64.uint8));
    }
    else {
        /* Use your code as a fallback or print an error message here */
        luid_get((void *)addressToWrite, IEEE802154_LONG_ADDRESS_LEN);
    }
}

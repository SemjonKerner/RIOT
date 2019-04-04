/*
 * Copyright (C) 2019 Hamburg University of Applied Sciences
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
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, April 2019
 *
 * @}
 */

#ifndef __RADIO_OW_H
#define __RADIO_OW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "radio.h"
#include "net/netdev.h"


typedef struct {
    radio_capture_cbt startFrame_cb;
    radio_capture_cbt endFrame_cb;
    radio_state_t state;
    netdev_t                 *dev;
} radio_vars_t;


#ifdef __cplusplus
}
#endif

#endif /* __RADIO_OW_H */
/** @} */
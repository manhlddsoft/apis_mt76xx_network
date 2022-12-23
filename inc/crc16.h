/*
 *	crc16.h - CRC-16 routine
 *
 * Implements the standard CRC-16:
 *   Width 16
 *   Poly  0x8005 (x^16 + x^15 + x^2 + 1)
 *   Init  0
 *
 * Copyright (c) 2005 Ben Gardner <bgardner@wabtec.com>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 2. See the file COPYING for more details.
 */

#ifndef __CRC16_H
#define __CRC16_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint16_t const crc16_table[256];

extern uint16_t crc16(const uint8_t *buffer, size_t len);

static inline uint16_t crc16_byte(uint16_t crc, const uint8_t data) {
    return (crc >> 8) ^ crc16_table[(crc ^ data) & 0xff];
}

#ifdef __cplusplus
}  // Extern C
#endif
#endif /* __CRC16_H */

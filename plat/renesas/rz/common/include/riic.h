/*
 * Copyright (c) 2022, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RIIC_H
#define RIIC_H

void riic_setup(uintptr_t i2c_base);
int32_t riic_write(uint8_t slave, uint8_t addr, uint8_t data);
int32_t riic_read(uint8_t slave, uint8_t addr, uint8_t *data);
int riic_dump(uint8_t slave, uint8_t addr, uint8_t *data, int length);

#endif /* RIIC_H */

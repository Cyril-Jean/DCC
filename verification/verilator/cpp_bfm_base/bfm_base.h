// MIT License
// Copyright (c) 2020 Cyril Jean
// SPDX-License-Identifier: MIT

#ifndef BFM_BASE_H
#define BFM_BASE_H

#include <stdint.h>
#include "test_jig_base.h"

class Bfm {
public:
    virtual void start_write_transaction(uint32_t address, uint32_t value);
    virtual void start_read_transaction(uint32_t address);
    virtual void write(uint32_t address, uint32_t value);
    virtual uint32_t read(uint32_t address);

    virtual void drive_bus();
};

#endif


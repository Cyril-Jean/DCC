// MIT license
// Copyright (c) 2019 Cyril Jean
// SPDX-License-Identifier: MIT

#ifndef APB_BFM
#define APB_BFM

#include "Vtop.h"

#include <stdint.h>

class ApbBfm {
private:
    enum ApbState {
        IDLE,
        WRITE_SETUP,
        WRITE_ACCESS,
        READ_SETUP,
        READ_ACCESS
    };

    ApbState state;

    uint32_t curr_address;
    uint32_t curr_value;
    Vtop * top;

public:
    ApbBfm(Vtop* top);
    ~ApbBfm();

    void write(uint32_t address, uint32_t value);
    uint32_t read(uint32_t address);
    void drive_bus();
};

#endif


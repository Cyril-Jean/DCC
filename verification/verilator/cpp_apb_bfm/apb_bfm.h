// MIT license
// Copyright (c) 2019 Cyril Jean
// SPDX-License-Identifier: MIT

#ifndef APB_BFM
#define APB_BFM

#include "Vtop.h"
#include "bfm_base.h"
#include <stdint.h>

typedef enum ApbState {
    IDLE,
    ENTER_IDLE,
    WRITE_SETUP,
    WRITE_ACCESS,
    READ_SETUP,
    READ_ACCESS
} ApbState_t;


class ApbBfm : public Bfm {
public:
    ApbBfm(Vtop* top, TestJigBase* test_jig);
    ~ApbBfm();

    void start_write_transaction(uint32_t address, uint32_t value);
    void start_read_transaction(uint32_t address);
    uint32_t read(uint32_t address);
    void write(uint32_t address, uint32_t value);
    void drive_bus();
    ApbState_t get_bus_state();

private:
    ApbState_t state;

    uint32_t curr_address;
    uint32_t curr_value;
    Vtop * top;
    TestJigBase* test_jig;
};

#endif


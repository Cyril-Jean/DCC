// MIT license
// Copyright (c) 2019 Cyril Jean
// SPDX-License-Identifier: MIT

#include "apb_bfm.h"

ApbBfm::ApbBfm(Vtop* in_top, TestJigBase* in_test_jig) {
    top = in_top;
    test_jig = in_test_jig;

    state = IDLE;
    top->psel = 0;
    top->penable = 0;
    top->pwrite = 0;
    top->pwdata = 0x00000000;
};

ApbBfm::~ApbBfm() {
};

//
// Initiate an APB bus write transaction. The transaction will progress every
// time drive_bus() is called each time the bus clock toggles.
//
void ApbBfm::start_write_transaction(uint32_t address, uint32_t value) {
    curr_address = address;
    curr_value = value;
    state = WRITE_SETUP;
};

//
// Initiate an APB bus read transaction. The bus transaction will progress
// every time drive_bus() is called and the bus clock toggles.
//
void ApbBfm::start_read_transaction(uint32_t address) {
    curr_address = address;
    state = READ_SETUP;
};


void ApbBfm::write(uint32_t address, uint32_t value) {
    start_write_transaction(address, value);
    for(int incc = 0; incc < 32; incc++) {
        test_jig->toggle_top_clk();
    }
}

//
// Perform a full read transaction. The test jig's clock is toggled a number of
// times as a result calling this function.
//
uint32_t ApbBfm::read(uint32_t address /*, TestJigBase* test_jig*/) {
    uint32_t value;
    start_read_transaction(address);

    do {
        test_jig->toggle_top_clk();
    } while(get_bus_state() != IDLE);

    value = top->prdata;

    return value;
}

//
// This function must be called every time the bus clock is toggled to move
// through the different stages of the APB bus transaction.
//
void ApbBfm::drive_bus() {
    switch(state) {
        case ENTER_IDLE:
            if(top->clk) {
                state = IDLE;
            }
            break;

        case IDLE:
            if(!top->clk) {
                top->psel = 0;
                top->penable = 0;
            };
            break;

        case WRITE_SETUP:
            if(!top->clk) {
                top->paddr = curr_address;
                top->pwdata = curr_value;
                top->pwrite = 1;
                top->psel = 1;
                state = WRITE_ACCESS;
            };
            break;

        case WRITE_ACCESS:
            if(!top->clk) {
                top->penable = 1;
                state = ENTER_IDLE;
            };
            break;

        case READ_SETUP:
            if(!top->clk) {
                top->pwrite = 0;
                top->paddr = curr_address;
                top->psel = 1;
                state = READ_ACCESS;
            }
            break;

        case READ_ACCESS:
            if(!top->clk) {
                top->penable = 1;
                state = ENTER_IDLE;
            };
            break;

        default:
            state = IDLE;
            break;
    };
};

ApbState_t ApbBfm::get_bus_state() {
    return state;
}


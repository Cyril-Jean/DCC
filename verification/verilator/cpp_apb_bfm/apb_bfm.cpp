// MIT license
// Copyright (c) 2019 Cyril Jean
// SPDX-License-Identifier: MIT

#include "apb_bfm.h"

ApbBfm::ApbBfm(Vtop * in_top) {
    top = in_top;

    state = IDLE;
    top->psel = 0;
    top->penable = 0;
    top->pwrite = 0;
    top->pwdata = 0x00000000;
};

ApbBfm::~ApbBfm() {
};

void ApbBfm::write(uint32_t address, uint32_t value) {
    curr_address = address;
    curr_value = value;
    state = WRITE_SETUP;
};

uint32_t ApbBfm::read(uint32_t address) {
    curr_address = address;
    state = READ_SETUP;

    
    return 0;
};

void ApbBfm::drive_bus() {
    switch(state) {
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

                state = IDLE;
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
                state = IDLE;
            };
            break;

        default:
            state = IDLE;
            break;
    };
};


// MIT License
// Copyright (c) 2020 Cyril Jean
// SPDX-License-IDentifier: MIT

#include <iostream>
#include "apb_tests.h"

#define RAM_BLOCK_BASE_OFFSET   0x800
#define TEST_PATTERN            0xdeadbeef
#define OFFSET_INCREMENT        4
#define PATTERN_DECREMENT       4

#define CLOCK_CYCLES            32

ApbTests::ApbTests() {
}


ApbTests::~ApbTests() {
}

int ApbTests::run(TestJig* test_jig) {
    uint32_t addr_offset = RAM_BLOCK_BASE_OFFSET;
    uint32_t pattern = TEST_PATTERN;

    for(int inc = 0; inc < 8; inc++) {
        test_jig->apb_bfm->write(addr_offset, pattern);
        for(int incc = 0; incc < CLOCK_CYCLES; incc++) {
            test_jig->toggle_top_clk();
        }

        addr_offset += OFFSET_INCREMENT;
        pattern -= PATTERN_DECREMENT;
    }

#if 1 
    addr_offset = RAM_BLOCK_BASE_OFFSET;
    pattern = TEST_PATTERN;
    for(int inc = 0; inc < 8; inc++) {
        uint32_t readback;
        readback =test_jig->apb_bfm->read(addr_offset);
        test_jig->advance_clk(CLOCK_CYCLES);
/*
        for(int incc = 0; incc < CLOCK_CYCLES; incc++) {
            test_jig->toggle_top_clk();
        }
*/
        if(readback != pattern) {
            std::cout << "Invalid APB bus read-back! Expected 0x" << std::hex 
                      << std::uppercase << pattern << ", found 0x" 
                      << readback  << std::endl;
        }
        addr_offset += OFFSET_INCREMENT;
        pattern -= PATTERN_DECREMENT;
    }
#endif
    return 0;
}

void ApbTests::drive_bus_a(int n_clock_cycles) {
    for(int inc = 0; inc < n_clock_cycles; inc++) {
//        bfm->drive_bus(top->clk);
//        std::cout << "APB tests drive bus: " << top->clk << std::endl;

        *main_time = *main_time + 1;

//        top->clk = !top->clk;
        top->clk = 1;
//        top->eval();
    }
}



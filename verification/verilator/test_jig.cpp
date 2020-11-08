// MIT License
// Copyright (c) 2020 Cyril Jean
// SPDX-License-Identifier: MIT

#include "test_jig.h"
#include "apb_bfm.h"

TestJig::TestJig(Vtop * in_top, VerilatedVcdC* in_tfp) {
    main_time = 0; 
    top = in_top;
    tfp = in_tfp;
    apb_bfm = new ApbBfm(top, this);
}


TestJig::~TestJig() {
}


void TestJig::toggle_top_clk() {
    main_time++;
    top->clk = !top->clk;
    apb_bfm->drive_bus();
    top->eval();
#if VM_TRACE
    if (tfp) tfp->dump(main_time);
#endif
    // Test track output
    if (top->clk == 0) {
        dcc_analyzer.SampleSignal(top->dcc_out);
    }
}

void TestJig::advance_clk(int n_clock_cycles) {

    for(int inc = 0; inc < (n_clock_cycles * 2); inc++) {
        toggle_top_clk();
    }
}

vluint64_t TestJig::get_main_time() {
    return main_time;
}


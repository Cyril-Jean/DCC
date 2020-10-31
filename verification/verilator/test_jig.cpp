// MIT License
// Copyright (c) 2020 Cyril Jean
// SPDX-License-IDentifier: MIT

#include "test_jig.h"

TestJig::TestJig(Vtop * in_top, VerilatedVcdC* in_tfp, ApbBfm* in_bfm) {
    main_time = 0; 
    top = in_top;
    tfp = in_tfp;
    apb_bfm = in_bfm;
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


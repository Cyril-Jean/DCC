// MIT License
// Copyright (c) 2020 Cyril Jean
// SPDX-License-Identifier: MIT

#ifndef TEST_JIG_H
#define TEST_JIG_H

#include "Vtop.h"
#include <verilated_vcd_c.h>
#include "bfm_base.h"
#include "dcc_analyzer.h"

class TestJig : public TestJigBase {
private:
    vluint64_t main_time;
    VerilatedVcdC* tfp;
    DccAnalyzer dcc_analyzer;

public:
    TestJig(Vtop* top, VerilatedVcdC* tfp);
    ~TestJig();

    Vtop* top;
    Bfm * apb_bfm;

    void toggle_top_clk();
    void advance_clk(int n_clock_cycles);
    vluint64_t get_main_time();
};

#endif


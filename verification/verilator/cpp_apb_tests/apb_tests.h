// MIT License
// Copyright (c) 2020 Cyril Jean
// SPDX-License-Identifier: MIT

#ifndef APB_TESTS_H
#define APB_TESTS_H

#include "Vtop.h"

#include <stdint.h>
#include "apb_bfm.h"
#include "Vtop.h"
#include "test_jig.h"

class ApbTests {
private:
    TestJig*        test_jig;
    Vtop *          top;
    ApbBfm *        bfm;
    vluint64_t *    main_time;

    void drive_bus_a(int n_clock_cycles);

public:
    ApbTests(TestJig* test_jig);
    ~ApbTests();

    int run(TestJig* test_jig);
};

#endif




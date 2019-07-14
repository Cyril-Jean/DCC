// MIT Licence
// Copyright (c) 2019 Cyril Jean
// SPDX-License-Identifier: MIT

#include <iostream>
#include "dcc_analyzer.h"

DccAnalyzer::DccAnalyzer() {
    state = IDLE;
    current_level = -1;
    current_level_count = 0;

    half_period_type = -1;
    half_period_count = 0;
};


DccAnalyzer::~DccAnalyzer() {
};

void DccAnalyzer::SampleSignal(int level) {
    int level_transition = 0;

    // Deal with line encoding
    if (current_level == UNKNOWN) {
        current_level = level;
        current_level_count = 0;
    };

    if (level == current_level) {
        ++current_level_count;
    } else {
        if (current_level) {
            std::cout << "High ";
        } else {
            std::cout << "Low  ";
        }
        std::cout << current_level_count << " clock cycles: " << std::endl;

        level_transition = 1;
    }

    // Decode protocol bits
    if (level_transition) {
        int previous_half_period;
        if (current_level_count == BASE_HALF_PERIOD_LENGTH) {
            previous_half_period = PROTOCOL_ONE;
         } else if (current_level_count == (2 * BASE_HALF_PERIOD_LENGTH)) {
            previous_half_period = PROTOCOL_ZERO;
        };

        if (previous_half_period == half_period_type) {
            ++half_period_count;
        } else {

                std::cout << " -> Protocol bit " << half_period_type 
                          << " received " << (half_period_count / 2) 
                          << " times" << std::endl;



            half_period_type = previous_half_period;
            half_period_count = 1;
        };

        current_level = level;
        current_level_count = 1;
    };

    // Protocol decoding
    switch(state) {
        case IDLE:
            break;
        case LOOK_FOR_FRAME_STATE:
            break;

        case FRAME_STARTED:
            break;
    };
};


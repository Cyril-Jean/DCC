// MIT Licence
// Copyright (c) 2019 Cyril Jean
// SPDX-License-Identifier: MIT

#include <iostream>
#include "dcc_analyzer.h"

//=============================================================================
//
//
DccAnalyzer::DccAnalyzer() {
    state = IDLE;
    current_level = -1;
    current_level_count = 0;

    half_period_type = -1;
    half_period_count = 0;
    previous_half_period_type = -1;
};

//=============================================================================
//
//
DccAnalyzer::~DccAnalyzer() {
};

//=============================================================================
//
//
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
        if (current_level_count == BASE_HALF_PERIOD_LENGTH) {
            half_period_type = PROTOCOL_ONE;
         } else if (current_level_count == (2 * BASE_HALF_PERIOD_LENGTH)) {
            half_period_type = PROTOCOL_ZERO;
        };

        if (previous_half_period_type == half_period_type) {
            ++half_period_count;
        } else {

                std::cout << " -> Protocol bit " << previous_half_period_type 
                          << " received " << (half_period_count / 2) 
                          << " times" << std::endl;

            decode_protocol();

            previous_half_period_type = half_period_type; 

            half_period_count = 1;
        };

        current_level = level;
        current_level_count = 1;
    };

};

//=============================================================================
//
//
void DccAnalyzer::decode_protocol() {
    // Protocol decoding
    switch(state) {
        case IDLE:
            state = LOOK_FOR_PREAMBLE;
            break;

        case LOOK_FOR_PREAMBLE:
            std::cout << " --> Look for preamble. type: "
                      << previous_half_period_type
                      << ", count: " << half_period_count
                      << std::endl;

            if((previous_half_period_type == PROTOCOL_ONE) &&
               (half_period_count >= 22)) {
                state = LOOK_FOR_FRAME_START;
                std::cout << " --> Preamble deteted" << std::endl;
            };
            break;

        case LOOK_FOR_FRAME_START:
            if((previous_half_period_type == PROTOCOL_ZERO) &&
               (half_period_count >= 2)) {
                std::cout << " --> Start of frame detected" << std::endl;
                state = LOOK_FOR_ADDRESS;;
            };
            break;

        case LOOK_FOR_ADDRESS:
            state = LOOK_FOR_PREAMBLE;
            break;

        case LOOK_FOR_COMMAND:
            state = LOOK_FOR_PREAMBLE;
            break;

        case FRAME_RECEIVED:
            state = IDLE;
            break;
    };

};


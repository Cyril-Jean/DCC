// MIT Licence
// Copyright (c) 2019 Cyril Jean
// SPDX-License-Identifier: MIT

#include <iostream>
#include "dcc_analyzer.h"

//=============================================================================
//
//
DccAnalyzer::DccAnalyzer() {
    state = LOOK_FOR_PREAMBLE;
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
    int rxed_half_period;

    rxed_half_period = decode_level(level);

    if ((rxed_half_period == LOW_HALF_PERIOD) ||
        (rxed_half_period == HIGH_HALF_PERIOD)) {

        decode_protocol(rxed_half_period);
    };
};

//=============================================================================
//
//
int DccAnalyzer::decode_level(int level) {
    int rxed_level = UNKNOWN;

    // Deal with line encoding
    if (current_level == UNKNOWN) {
        current_level = level;
        current_level_count = 0;
    };

    if (level == current_level) {
        ++current_level_count;
    } else {
        report_level_change(current_level, current_level_count);
        if (current_level_count == BASE_HALF_PERIOD_LENGTH) {
            rxed_level = HIGH_HALF_PERIOD;
         } else if (current_level_count >= (2 * BASE_HALF_PERIOD_LENGTH)) {
            rxed_level = LOW_HALF_PERIOD;
        };

        current_level = level;
        current_level_count = 1;
    };

    return rxed_level;
};

//=============================================================================
//
//
void DccAnalyzer::decode_protocol(int half_period) {
    // Protocol decoding
    switch(state) {
        case LOOK_FOR_PREAMBLE:
            if (half_period == PROTOCOL_ONE) {
                ++half_period_count;
                if (half_period_count >= 22) {
                    half_period_count = 0;
                    rx_byte_index = 0;
                    previous_half_period_type = UNKNOWN;
                    state = LOOK_FOR_SEPARATOR;
                    report_preamble_detected();
                }
            } else {
                half_period_count = 0;
            }
            break;

        case LOOK_FOR_SEPARATOR:
            switch (previous_half_period_type) {
                case PROTOCOL_ZERO:
                    if (half_period == PROTOCOL_ZERO) {
                        report_start_bit_found();
                        byte_half_period_count = 0;
                        half_period_count = 0;
                        state = RETRIEVE_BYTE;
                    }
                    break;

                case PROTOCOL_ONE:
                    if (half_period == PROTOCOL_ONE) {
                        report_frame_received();
                        byte_half_period_count = 0;
                        half_period_count = 0;
                        state = LOOK_FOR_PREAMBLE;
                    }
                    break;

                case UNKNOWN:
                    previous_half_period_type = half_period;
                    break;

                default:
                    state = LOOK_FOR_PREAMBLE;
                    break;
            }
            break;

        case RETRIEVE_BYTE:
            byte_half_period[byte_half_period_count] = half_period;
            ++byte_half_period_count;
            if(byte_half_period_count >= 16) {
                int rxed_byte;
                rxed_byte = get_byte_from_half_periods(byte_half_period);
                if (rxed_byte < 0) {
                    report_invalid_address();
                    half_period_count = 0; 
                    state = LOOK_FOR_PREAMBLE;
                } else {
                    if (rx_byte_index < 6) {
                        rxed_bytes[rx_byte_index] = rxed_byte;
                        ++rx_byte_index;
                    }
                    previous_half_period_type = UNKNOWN;
                    state = LOOK_FOR_SEPARATOR;
                    report_rxed_byte(rxed_byte);
                }
            }
            break;

        default:
            state = LOOK_FOR_PREAMBLE;
            break;
    };
};

//=============================================================================
//
//
int DccAnalyzer::get_byte_from_half_periods(int rx_half_periods[16]) {

    int byte = 0;

    for(int inc = 0; inc < 16; inc = inc + 2) {

        if(rx_half_periods[inc] != rx_half_periods[inc + 1]) {
            return -1;
        }

        byte = (byte << 1) | rx_half_periods[inc];
    }
    return byte;
};

//=============================================================================
//
//
void DccAnalyzer::report_level_change
(
    int level,
    int count)
{
    if (verbosity == HIGH) {
        if (level) {
            std::cout << "High ";
        } else {
            std::cout << "Low  ";
        }
        std::cout << count << " clock cycles: " << std::endl;
    };
}

void DccAnalyzer::report_preamble_detected() {
    if (verbosity >= MEDIUM) {
        std::cout << " --> Preamble detected" 
                  << " -----------------------------"
                  << std::endl;
    }
}

void DccAnalyzer::report_sof_detected() {
    if (verbosity >= MEDIUM) {
        std::cout << " --> Start of frame detected" 
                  << " -------------------------"
                  << std::endl;
    }
}

void DccAnalyzer::report_invalid_address() {
    if (verbosity >= MEDIUM) {
        std::cout << " --> Invalid address received" 
                  << " -------------------------"
                  << std::endl;
    }
}

void DccAnalyzer::report_rxed_byte(int value) {
    if (verbosity >= MEDIUM) {
        std::cout << " --> Byte received: 0x"
                  << std::hex << std::uppercase << value
                  << " -------------------------"
                  << std::endl;
    }
}

void DccAnalyzer::report_cmd_start() {
    if (verbosity >= MEDIUM) {
        std::cout << " --> Command start detected" 
                  << " ------------------------"
                  << std::endl;
    }
}

void DccAnalyzer::report_start_bit_found() {
    if (verbosity >= MEDIUM) {
        std::cout << " --> Start bit found" 
                  << " -----------------------------"
                  << std::endl;
    }
}

void DccAnalyzer::report_frame_received() {
    std::cout << " --> Frame received: ";

    for (int inc = 0; inc < rx_byte_index; inc++) {
        std::cout << std::hex << std::uppercase << "0x"
                  << rxed_bytes[inc] <<" ";
    }

    std::cout << " -------------------------------"
              << std::endl;


}


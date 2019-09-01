// MIT License
// Copyright (c) 2019 Cyril Jean
// SPDX-License-Identifier: MIT

#ifndef DCC_ANALYZER_H
#define DCC_ANALYZER_H

class DccAnalyzer {
private:
    enum AnalyzerState {
        LOOK_FOR_PREAMBLE,
        LOOK_FOR_SEPARATOR,
        RETRIEVE_BYTE
    };

    enum VerboseLevel {
        LOW,
        MEDIUM,
        HIGH
    };

    enum AnalyzerState state;
    enum VerboseLevel verbosity = LOW;

    static const int UNKNOWN    = -1;
    static const int LEVEL_HIGH = 1;
    static const int LEVEL_LOW  = 0;

    static const int LOW_HALF_PERIOD = 0;
    static const int HIGH_HALF_PERIOD = 1;

    static const int PROTOCOL_ZERO = 0;
    static const int PROTOCOL_ONE = 1;

    int current_level;
    int current_level_count;

    int half_period_count;
    int half_period_type;
    int zero_half_period_count;

    int error_code;

    int previous_half_period_type;
    int byte_half_period[16];
    int byte_half_period_count;

    static const int prescaled_length = 4;
    static const int BASE_HALF_PERIOD_LENGTH = 8;

    int rxed_bytes[6];
    int rx_byte_index;
    int decode_level(int level);

    void decode_protocol(int half_period);

    int get_byte_from_half_periods(int rx_half_periods[16]);

    void report_level_change(int level, int count);
    void report_preamble_detected();
    void report_sof_detected();
    void report_invalid_address();
    void report_rxed_byte(int value);
    void report_cmd_start();
    void report_start_bit_found();
    void report_frame_received();

public:
    DccAnalyzer();
    ~DccAnalyzer();

    void SampleSignal(int level);
    int RxedLength();
    int * RxedFrame();
};

#endif


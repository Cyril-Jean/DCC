// MIT License
// Copyright (c) 2019 Cyril Jean
// SPDX-License-Identifier: MIT

#ifndef DCC_ANALYZER_H
#define DCC_ANALYZER_H

class DccAnalyzer {
private:
    enum AnalyzerState {
        IDLE,
        LOOK_FOR_PREAMBLE,
        LOOK_FOR_FRAME_START,
        LOOK_FOR_ADDRESS,
        LOOK_FOR_COMMAND,
        FRAME_RECEIVED
    };

    enum AnalyzerState state;

    static const int UNKNOWN    = -1;
    static const int LEVEL_HIGH = 1;
    static const int LEVEL_LOW  = 0;

    static const int PROTOCOL_ZERO = 0;
    static const int PROTOCOL_ONE = 1;

    int current_level;
    int current_level_count;

    int half_period_count;
    int half_period_type;

    int previous_half_period_type;

    static const int prescaled_length = 4;
    static const int BASE_HALF_PERIOD_LENGTH = 8;

    void decode_protocol();

public:
    DccAnalyzer();
    ~DccAnalyzer();

    void SampleSignal(int level); 
};

#endif


/*
 * main.cpp
 */
#include "dcc_analyzer.h"
#include <iostream>

const int HALF_PERIOD_LENGTH = 8;
const int NB_OF_TESTS = 4;

typedef struct {
	int length;
	int * sequence;
} bit_sequence_t;

typedef struct {
	bit_sequence_t * bit_sequence;
	int expected_byte_length;
	int * expected_frame_bytes;
} test_description_t;

//-----------------------------------------------------------------------------
int simple_cmd_1_bs[41] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0,
		1, 0, 1, 0, 1, 0, 1, 0,
		0,
		1, 1, 0, 0, 1, 1, 0, 0,
		0,
		1, 1, 1, 0, 0, 1, 1, 0,
		1,
		0, 0
};

bit_sequence_t simple_cmd_1 = {
	sizeof(simple_cmd_1_bs) / sizeof(int),
	simple_cmd_1_bs
};

int simple_cmd_1_exp_frame[3] = {
		0xAA, 0xCC, 0xE6
};

test_description_t simple_cmd_1_test = {
	&simple_cmd_1,
	sizeof(simple_cmd_1_exp_frame) / sizeof(int),
	simple_cmd_1_exp_frame
};

//-----------------------------------------------------------------------------
int simple_cmd_2_bs[41] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0,
		0, 1, 0, 1, 0, 1, 0, 1,
		0,
		0, 0, 1, 1, 0, 0, 1, 1,
		0,
		0, 1, 1, 0, 0, 1, 1, 0,
		1,
		0, 0
};

bit_sequence_t simple_cmd_2 = {
	sizeof(simple_cmd_2_bs) / sizeof(int),
	simple_cmd_2_bs
};

int simple_cmd_2_exp_frame[3] = {
		0x55, 0x33, 0x66
};

test_description_t simple_cmd_2_test = {
	&simple_cmd_2,
	sizeof(simple_cmd_2_exp_frame) / sizeof(int),
	simple_cmd_2_exp_frame
};

//-----------------------------------------------------------------------------
int simple_cmd_3_bs[50] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0,
		1, 1, 1, 1, 0, 1, 0, 1,
		0,
		0, 0, 1, 1, 0, 0, 1, 1,
		0,
		1, 0, 1, 1, 1, 0, 0, 1,
		0,
		0, 1, 1, 1, 1, 1, 1, 1,
		1,
		0, 0
};

bit_sequence_t simple_cmd_3 = {
	sizeof(simple_cmd_3_bs) / sizeof(int),
	simple_cmd_3_bs
};

int simple_cmd_3_exp_frame[4] = {
		0xF5, 0x33, 0xB9, 0x7F
};

test_description_t simple_cmd_3_test = {
	&simple_cmd_3,
	sizeof(simple_cmd_3_exp_frame) / sizeof(int),
	simple_cmd_3_exp_frame
};


//-----------------------------------------------------------------------------
int long_preamble_bs[53] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0,
		1, 1, 1, 1, 0, 1, 0, 1,
		0,
		0, 0, 1, 1, 0, 0, 1, 1,
		0,
		1, 0, 1, 1, 1, 0, 0, 1,
		0,
		0, 1, 1, 1, 1, 1, 1, 1,
		1,
		0, 0
};

bit_sequence_t long_preamble = {
	sizeof(long_preamble_bs) / sizeof(int),
	long_preamble_bs
};


int long_preamble_exp_frame[4] = {
		0xF5, 0x33, 0xB9, 0x7F
};

test_description_t long_preamble_test = {
	&long_preamble,
	sizeof(long_preamble_exp_frame) / sizeof(int),
	long_preamble_exp_frame
};

//-----------------------------------------------------------------------------
bit_sequence_t * test_sequences[NB_OF_TESTS] = {
	&simple_cmd_1,
	&simple_cmd_2,
	&simple_cmd_3,
	&long_preamble
};

test_description_t * tests[NB_OF_TESTS] = {
	&simple_cmd_1_test,
	&simple_cmd_2_test,
	&simple_cmd_3_test,
	&long_preamble_test
};

//-----------------------------------------------------------------------------
void send_valid_one(DccAnalyzer * analyzer) {
	int inc;

	for (inc = 0; inc < HALF_PERIOD_LENGTH; inc++) {
		analyzer->SampleSignal(1);
	}

	for (inc = 0; inc < HALF_PERIOD_LENGTH; inc++) {
		analyzer->SampleSignal(0);
	}
}

//-----------------------------------------------------------------------------
void send_valid_zero(DccAnalyzer * analyzer) {
	int inc;

	for (inc = 0; inc < (HALF_PERIOD_LENGTH * 2); inc++) {
		analyzer->SampleSignal(1);
	}

	for (inc = 0; inc < (HALF_PERIOD_LENGTH * 2); inc++) {
		analyzer->SampleSignal(0);
	}
}

//-----------------------------------------------------------------------------
void send_bit_sequence(DccAnalyzer * analyzer, bit_sequence_t * sequence) {

	for (int inc = 0;  inc < sequence->length; inc++) {
		if (sequence->sequence[inc] == 0) {
			send_valid_zero(analyzer);
		}
		else {
			send_valid_one(analyzer);
		}
	}
}

//=============================================================================
int main() {
	DccAnalyzer analyzer;
	bool passed;

	for (int test_idx = 0; test_idx < NB_OF_TESTS; test_idx++) {
		passed = true;
		int rxed_length;
		int * rxed_frame;

		send_bit_sequence(&analyzer, tests[test_idx]->bit_sequence);
		//
		// Check received frame length
		//
		rxed_length = analyzer.RxedLength();
		if (rxed_length != tests[test_idx]->expected_byte_length) {
			passed = false;
		}
		//
		// Check received frame content
		//
		rxed_frame = analyzer.RxedFrame();
		for (int byte_idx = 0; byte_idx < rxed_length; byte_idx++) {
			if (rxed_frame[byte_idx] != tests[test_idx]->expected_frame_bytes[byte_idx]) {
				passed = false;
			}
		}
		//
		// Report test outcome
		//
		if (passed) {
			std::cout << "    --- PASSED ---\n" << std::endl;
		} else {
			std::cout << "    !!! FAILED !!!\n" << std::endl;
		}
	}

	return(0);
}



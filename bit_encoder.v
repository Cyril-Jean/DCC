// MIT License
// Copyright (c) 2019 Cyril Jean
// SPDX-License-Identifier: MIT

module bit_encoder
  (
    input      clk,
    input      reset_n,
    input      next_bit_in,
    output     ack,
    output reg encoded_out
  );

  parameter idle        = 3'b000;
  parameter zero_low    = 3'b001;
  parameter zero_low_2  = 3'b010;
  parameter zero_high   = 3'b011;
  parameter zero_high_2 = 3'b100;
  parameter one_low     = 3'b101;
  parameter one_high    = 3'b110;

  reg       next_bit;
  reg [2:0] state;
  reg [7:0] prescaler;

  wire ser_clk;

  // Serial port prescaler
  assign ser_clk = prescaler[2];
  always @ (posedge clk)
    begin
//      if (reset_n == 1'b0)
//        prescaler <= 0;
//      else
      prescaler <= prescaler + 1;
    end

  // State machine
  always @ (posedge ser_clk)
    begin
      if (reset_n == 1'b0)
        begin
          state <= zero_low;
          next_bit <= 1'b0;
          encoded_out <= 1'b0;
          ack <= 1'b0;
        end
      else
        case (state)
          zero_low:
            state <= zero_low_2;

          zero_low_2:
            begin
              encoded_out <= 1'b1;
              next_bit <= next_bit_in;
              ack <= 1'b1;
              state <= zero_high;
            end

          zero_high:
            begin
              ack <= 1'b0;
              state <= zero_high_2;
            end

          one_low:
            begin
              encoded_out <= 1'b1;
              ack <= 1'b1;
              next_bit <= next_bit_in;
              state <= one_high;
            end

          one_high, zero_high_2:
            begin
                if (next_bit == 1'b1)
                begin
                  encoded_out <= 1'b0;
                  state <= one_low;
                end
              else
                begin
                  encoded_out <= 1'b0;
                  state <= zero_low;
                end
              ack <= 1'b0;
            end

          default:
            begin
              encoded_out <= 1'b0;
              state <= zero_low;
            end
        endcase
    end

endmodule


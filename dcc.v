// MIT License
// Copyright (c) 2019 Cyril Jean
// SPDX-License-Identifier: MIT

module dcc
  (
    input             clk,
    input             reset_n,
    output reg [9:0]  cmd_index,
    input      [31:0] cmd_word,
    output reg        track_out
  );

  parameter idle         = 4'b0000;
  parameter preamble     = 4'b0010;
  parameter packet_start = 4'b0100;
  parameter address      = 4'b0111;
  parameter instr_start  = 4'b1000;
  parameter instruction  = 4'b1011;
  parameter error_start  = 4'b1100;
  parameter error_detect = 4'b1111;

  parameter SHIFT_BITS   = 4'b0001;

  reg [3:0]  state;
  reg [15:0] addr_cmd;
  reg [15:0] shift_out;
  reg        next_bit;
  reg        ack_sync;
  reg [7:0]  bit_count;
//  reg [7:0]  address_byte;
//  reg [7:0]  cmd_byte;

  wire ser_clk;
  wire ack;

  //
  always @(posedge clk)
    begin
      if (reset_n == 1'b0)
        begin
          cmd_index <= 0;
          addr_cmd <= 16'hAFF5;
          ack_sync <= 1'b0;
  //        address_byte <= 8'hFF;
  //        cmd_byte <= 8'hFF;
        end
    end

  //---------------------------------------------------------------------------
  always @(posedge clk)
    begin
      ack_sync <= ack;
    end

  //---------------------------------------------------------------------------
  always @(posedge clk)
    begin
      if ((state & SHIFT_BITS) == 4'b0001)
        begin
        if (ack & !ack_sync)
          begin
            next_bit <= shift_out[0];
            shift_out[15:0] <= {1'b0, shift_out[15:1]};
            bit_count <= bit_count + 1'b1;
          end
        end
    end

  //---------------------------------------------------------------------------
  always @(posedge clk)
    begin
      if (reset_n == 1'b0)
        begin
          state <= idle;
          shift_out <= 16'h0000;
          next_bit <= 1'b0;
        end
      else
        begin
          case (state)
            idle:
              begin
                state <= preamble;
                bit_count <= 8'b0;
                next_bit <= 1'b1;
              end

            preamble:
                if (ack & !ack_sync)
                  begin
                    bit_count <= bit_count + 1;
                    if (bit_count == 8'd10)
                      begin
                        state <= packet_start;
                        next_bit <= 1'b0;
                      end
                  end

            packet_start:
                if (ack & !ack_sync)
                  begin
                    state <= address;
//                    next_bit <= address_byte[0];
//                    shift_out <= {9'h0, address_byte[7:1]};
                    next_bit <= cmd_word[8];
                    shift_out <= {9'h0, cmd_word[15:9]};

                    bit_count <= 8'b0;
                 end

            address:
                if (bit_count == 8'd8)
                  begin
                    state <= instr_start;
                    next_bit <= 1'b0;
                  end

            instr_start:
                if (ack & !ack_sync)
                  begin
                    state <= instruction;
//                    next_bit <= cmd_byte[0];
//                    shift_out <= {9'h0, cmd_byte[7:1]};
                    next_bit <= cmd_word[0];
                    shift_out <= {9'h0, cmd_word[7:1]};

                    bit_count <= 8'b0;
                  end

            instruction:
                if (bit_count == 8'd8)
                  begin
                    state <= error_start;
                    next_bit <= 1'b0;
                  end

            error_start:
                if (ack & !ack_sync)
                  begin
                    state <= error_detect;
//                    next_bit <= address_byte[0] ^ cmd_byte[0];
//                    shift_out <= {9'b0, address_byte[7:1] ^ cmd_byte[7:1]};
                    next_bit <= cmd_word[8] ^ cmd_word[0];
                    shift_out <= {9'b0, cmd_word[15:9] ^ cmd_word[7:1]};

                    bit_count <= 8'd0;
                  end

            error_detect:
                if (bit_count == 8'd8)
                  begin
                    state <= preamble;
                    next_bit <= 1'b1;
                    bit_count <= 8'd0;

                    cmd_index <= cmd_index + 1'b1;
                  end

            default:
              state <= idle;
          endcase
        end
    end

  //
  bit_encoder bit_encoder(.clk(clk),
                          .reset_n(reset_n),
                          .next_bit(next_bit),
                          .ack(ack),
                          .encoded_out(track_out));

endmodule


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
  parameter start_bit    = 4'b0100;
  parameter frame_byte   = 4'b0111;
  parameter error_detect = 4'b1111;
  parameter end_bit      = 4'b1110;

  parameter SHIFT_BITS   = 4'b0001;

  parameter empty       = 2'b00;
  parameter valid_byte  = 2'b01;
  parameter last_byte   = 2'b11;

  reg [3:0]  state;
  reg [6:0] shift_out;
  reg        next_bit;
  reg        ack_sync;
  reg [7:0]  bit_count;

  reg [7:0]  next_byte;
  reg [1:0]  next_byte_cmd;
  reg        is_last_byte;
  reg [1:0]  subword_idx;
  reg [7:0]  err_det_byte;  // Error detection byte: XOR of frame bytes

  reg        back_to_start;

  wire ack;
  wire track_out;
  wire dcc_clk;

//---------------------------------------------------------------------------
  always @(posedge clk or negedge reset_n)
    begin
      if (!reset_n)
        begin
          state <= idle;
          shift_out <= 7'h00;
          next_bit <= 1'b1;
          subword_idx <= 2'b00;
          err_det_byte <= 8'b0;
          cmd_index <= 0;
          ack_sync <= 1'b0;
          next_byte <= 8'b0;
          next_byte_cmd <= empty;
          is_last_byte <= 1'b0;
          back_to_start <= 0;
        end
      else
        begin
        
          if ((state & SHIFT_BITS) == 4'b0001)
            begin
              if (ack & !ack_sync)
                begin
                  next_bit <= shift_out[6];
                  shift_out[6:0] <= {shift_out[5:0], 1'b0};
                  bit_count <= bit_count + 1'b1;
                end
              end
      
          ack_sync <= ack;

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
                    if (bit_count == 8'd16)
                      begin
                        state <= start_bit;
                        next_bit <= 1'b0;
                        subword_idx <= 2'b0;
                        next_byte <= cmd_word[7:0];
                        next_byte_cmd <= cmd_word[25:24];
                        err_det_byte <= 8'd0;
                      end
                  end

            start_bit:
                if (ack & !ack_sync)
                  begin
                    if (is_last_byte == 1'b1)
                      begin
                        state <= error_detect;
                        is_last_byte <= 1'b0;
                        next_bit <= err_det_byte[7];
                        shift_out <= err_det_byte[6:0];
                      end
                    else
                      begin
                        state <= frame_byte;
                        next_bit <= next_byte[7];
                        shift_out <= next_byte[6:0];
                        err_det_byte <= err_det_byte ^ next_byte[7:0];
                      end

                    if (next_byte_cmd == last_byte)
                      begin
                        is_last_byte <= 1'b1;
                      end

                    bit_count <= 8'b0;
                    subword_idx <= subword_idx + 1'b1;
                 end

            frame_byte:
                if (bit_count == 8'd8)
                  begin
                    state <= start_bit;
                    next_bit <= 1'b0;
                    // Setup for next frame byte
                    case (subword_idx)
                      2'd0:
                        begin
                          next_byte <= cmd_word[7:0];
                          next_byte_cmd <= cmd_word[25:24];
                        end

                      2'd1:
                        begin
                          next_byte <= cmd_word[15:8];
                          next_byte_cmd <= cmd_word[27:26];
                        end

                      2'd2:
                        begin
                          next_byte <= cmd_word[23:16];
                          next_byte_cmd <= cmd_word[29:28];
                        end

                      default:
                        begin
                          next_byte <= 8'd0;
                          next_byte_cmd <= 2'd0;
                        end
                    endcase
                    back_to_start <= cmd_word[31];
                  end

            error_detect:
                if (bit_count == 8'd8)
                  begin
                    state <= end_bit;
                    next_bit <= 1'b1;
                    bit_count <= 8'd0;
                    if (back_to_start)
                      cmd_index <= 10'b0;
                    else
                      cmd_index <= cmd_index + 1'b1;
                  end

            end_bit:
                if (ack & !ack_sync)
                  begin
                    state <= preamble;
                    bit_count <= 8'd0;
                  end

            default:
              state <= idle;
          endcase
        end
    end

  //
  dcc_clk_gen clk_gen(.clk(clk),
                      .reset_n(reset_n),
                      .bus_clk(dcc_clk));
  //
  bit_encoder bit_encoder_inst(.dcc_clk(dcc_clk),
                          .reset_n(reset_n),
                          .next_bit_in(next_bit),
                          .ack(ack),
                          .encoded_out(track_out));

endmodule

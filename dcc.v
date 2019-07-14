// MIT License
// Copyright (c) 2019 Cyril Jean
// SPDX-License-Identifier: MIT

module dcc
  (
    input      clk,
    input      reset_n,
    output reg track_out
  );

  reg [15:0] addr_cmd;
  reg [15:0] shift_out;
  reg        next_bit;
  reg        ack_sync;

  wire ser_clk;
  wire ack;

  //
  always @(posedge clk)
    begin
      if (reset_n == 1'b0)
        begin
          addr_cmd <= 16'hAFF5;
          ack_sync <= 1'b0;
        end
    end

  //
  always @(posedge clk)
    begin
      ack_sync <= ack;
    end

  //
  always @(posedge clk)
    begin
      if (reset_n == 1'b0)
        shift_out <= 16'hAFF5;
      else
        begin
          if (ack & !ack_sync)
          begin
            next_bit <= shift_out[0];
            shift_out[15:0] <= {1'b0, shift_out[15:1]};
          end
        end
    end

  //
  bit_encoder bit_encoder(.clk(clk),
                          .reset_n(reset_n),
                          .next_bit(next_bit),
                          .ack(ack),
                          .encoded_out(track_out));

endmodule


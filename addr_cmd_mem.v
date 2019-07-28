// MIT License
// Copyright (c) 2019 Cyril Jean
// SPDX-License-Identifier: MIT

module addr_cmd_mem
  (
    input reset_n,
    // Host port
    input         clk_a,
    input  [31:0] dat_in_a,
    input  [9:0]  address_a,
    output [31:0] dat_out_a,
    input         wr_a,
    // Encoder port
    input         clk_b,
    input  [31:0] dat_in_b,
    input  [9:0]  address_b,
    output [31:0] dat_out_b,
    input         wr_b
  );

  reg [31:0]  memory[512:0];
  reg [31:0]  dat_out_a;
  reg [31:0]  dat_out_b;

  //
  // Host port
  //
  always @ (posedge clk_a)
    begin
      dat_out_a <= memory[address_a];
      if (wr_a)
        begin
          dat_out_a <= dat_in_a;
          memory[address_a] <= dat_in_a;
        end 
    end

  //
  // Encoder port
  //
  always @ (posedge clk_b)
    begin
      dat_out_b <= memory[address_b];
      if (wr_b)
        begin
          dat_out_b <= dat_in_b;
          memory[address_b] <= dat_in_b;
        end
    end

endmodule


// MIT License
// Copyright (c) 2020 Cyril Jean
// SPDX-License_Identifier: MIT

module dcc_clk_gen
  (
    input   clk,
    input   reset_n,
    output  bus_clk
  );

  reg [12:0] prescaler;

  assign bus_clk = prescaler[2];

  always @(posedge clk or negedge reset_n)
    begin
      if (!reset_n)
        begin
          prescaler <= 13'b0;
        end
      else
        begin
          prescaler <= prescaler + 1;
        end
    end

endmodule



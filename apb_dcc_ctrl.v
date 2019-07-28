// MIT License
// Copyright (c) 2019 Cyril Jean
// SPDX-License-IDentifier: MIT

module apb_dcc_ctrl
  (
    input         presetn,
    input         pclk,
    input         psel,
    input         penable,
    input         pwrite,
    output        pslverr,
    output        pready,
    input  [11:0] paddr,
    input  [31:0] pwdata,
    output [31:0] prdata,
    output        dcc_out
  );

  reg [7:0] ctrl_reg;

  wire write_from_apb;
  wire [9:0] cmd_index;
  wire [31:0] cmd_word;
//  wire [31:0] loopback1;

  assign pslverr = 1'b0;
  assign pready= 1'b1;

  //---------------------------------------------------------------------------
  // APB peripheral reset
  //
  always @(posedge pclk)
    begin
      if (presetn == 1'b0)
        begin
          ctrl_reg <= 8'h0;
        end
    end

  //---------------------------------------------------------------------------
  // Control register
  //
  always @(posedge pclk)
    begin
      if ((psel == 1'b1) & (pwrite == 1'b1) & (penable == 1'b1))
        begin
          if (paddr == 12'h800) ctrl_reg <= 8'h0;
        end
    end

  //---------------------------------------------------------------------------
  // Address/Commands memory
  //
  assign write_from_apb = (psel == 1'b1) 
                          & (pwrite == 1'b1) 
                          & (penable == 1'b1) 
                          & (paddr[11] == 1'b1);

  addr_cmd_mem addr_cmd_mem1
    (
      .reset_n(presetn),
      .clk_a      (pclk),
      .dat_in_a   (pwdata),
      .address_a  ({1'b0, paddr[10:2]}),
      .dat_out_a  (prdata),
      .wr_a       (write_from_apb),
      .clk_b      (pclk),
      .dat_in_b   (pwdata),
      .address_b  (cmd_index),
      .dat_out_b  (cmd_word),
      .wr_b       (1'b0)
    );

//-----------------------------------------------------------------------------
// DCC
//
dcc dcc1 (.clk(pclk),
          .reset_n(presetn),
          .track_out(dcc_out),
          .cmd_index(cmd_index),
          .cmd_word(cmd_word));    

endmodule;


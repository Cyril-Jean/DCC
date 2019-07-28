// =============================================================================
// Top level test bench based on Verilator example.
// =============================================================================

module top
  (
    // Declare some signals so we can see how I/O works
    input         clk,
    input         reset_l,

    // APB slave interface
    input         presetn,
    input         psel,
    input         penable,
    input         pwrite,
    output        pslverr,
    output        pready,
    input  [11:0] paddr,
    input  [31:0] pwdata,
    output [31:0] prdata,

    //
    output        dcc_out
  );

  reg [31:0] elapsed_clock;
  reg        reset_sync;

  always @(posedge clk)
    begin
      if (reset_l)
        reset_sync <= 1'b1;
      else
        reset_sync <= 1'b0;
     end

  // APB DCC controller
  apb_dcc_ctrl apb_dcc_ctrl1 (.presetn(reset_l),
                              .pclk(clk),
                              .psel(psel),
                              .penable(penable),
                              .pwrite(pwrite),
                              .pslverr(pslverr),
                              .pready(pready),
                              .paddr(paddr),
                              .pwdata(pwdata),
                              .prdata(prdata),
                              .dcc_out(dcc_out));

  // Print some stuff as an example
  initial begin
    $display("[%0t] Model running...\n", $time);
  end

  // Lazy test duration control. Run for a specific number of cock cycles
  always @ (posedge clk)
    begin
      if (reset_l == 1'b0)
        begin
          elapsed_clock <= 32'b0;
        end
      else
        begin
          elapsed_clock <= elapsed_clock + 1;
          if (elapsed_clock[11] == 1'b1) $finish;
        end
    end

endmodule

// =============================================================================
// Top level test bench based on Verilator example.
// =============================================================================

module top
  (
   // Declare some signals so we can see how I/O works
   input         clk,
//   input         fastclk,
   input         reset_l,

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

   // DCC module
   dcc dcc (.clk(clk),
            .reset_n(reset_sync),
            .track_out(dcc_out));

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

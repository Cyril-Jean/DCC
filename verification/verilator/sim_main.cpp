//=============================================================================
// Verilog C++ test bench.
//=============================================================================

// Include common routines
#include <verilated.h>

// Include model header, generated from Verilating "top.v"
#include "Vtop.h"

// If "verilator --trace" is used, include the tracing class
#if VM_TRACE
# include <verilated_vcd_c.h>
#endif

#include "dcc_analyzer.h"
#include "apb_bfm.h"
#include "apb_tests.h"
#include "test_jig.h"

TestJig * test_jig = NULL; 

// Called by $time in Verilog
double sc_time_stamp() {
    if (test_jig) return test_jig->get_main_time();
    return 0;
}

int main(int argc, char** argv, char** env) {
    // Prevent unused variable warnings
    if (0 && argc && argv && env) {}

    // Set debug level, 0 is off, 9 is highest presently used
    // May be overridden by commandArgs
    Verilated::debug(0);

    // Randomization reset policy
    // May be overridden by commandArgs
    Verilated::randReset(2);

    // Pass arguments so Verilated code can see them, e.g. $value$plusargs
    // This needs to be called before you create any model
    Verilated::commandArgs(argc, argv);

    // Construct the Verilated model, from Vtop.h generated from Verilating "top.v"
    Vtop* top = new Vtop;  // Or use a const unique_ptr, or the VL_UNIQUE_PTR wrapper

    // APB BFM relies on Vtop to expose one APB interface to its top level.
    ApbBfm * apb_bfm = new ApbBfm(top);

#if VM_TRACE
    // If verilator was invoked with --trace argument,
    // and if at run time passed the +trace argument, turn on tracing
    VerilatedVcdC* tfp = NULL;
    const char* flag = Verilated::commandArgsPlusMatch("trace");
    if (flag && 0==strcmp(flag, "+trace")) {
        Verilated::traceEverOn(true);  // Verilator must compute traced signals
        VL_PRINTF("Enabling waves into logs/vlt_dump.vcd...\n");
        tfp = new VerilatedVcdC;
        top->trace(tfp, 99);  // Trace 99 levels of hierarchy
        Verilated::mkdir("logs");
        tfp->open("logs/vlt_dump.vcd");  // Open the dump file
    }
#endif

    test_jig = new TestJig(top, tfp, apb_bfm);

    // Set some inputs
    top->reset_l = !1;
    top->presetn = !1;
//    top->fastclk = 0;
    top->clk = !0;

    // Keep the reset active for a few clock cycles before really starting the fun.
    top->reset_l = !1;  // Assert reset
    top->presetn = !1;

    test_jig->advance_clk(8);

    top->reset_l = !0;  // Deassert reset
    top->presetn = !0;

    test_jig->advance_clk(4);

    ApbTests apb_tests;
    apb_tests.run(test_jig);


    // Simulate until $finish
    while (!Verilated::gotFinish()) {

        if (test_jig->get_main_time() == 20) {
//            apb_tests.run(test_jig);
        };
 
    
        if (test_jig->get_main_time() == 800) {
            apb_bfm->write(0x800, 0x0DA7A6A5);
        };
        if (test_jig->get_main_time() == 900) {
            apb_bfm->write(0x804, 0x35020304);
        };
         if (test_jig->get_main_time() == 1000) {
            apb_bfm->read(0x800);
        };
         if (test_jig->get_main_time() == 1100) {
            apb_bfm->write(0x808, 0xB5123344);
        };
       


//        apb_bfm->drive_bus(top->clk);

        test_jig->toggle_top_clk();

        // Read outputs
#if 0
        VL_PRINTF("[%" VL_PRI64 "d] clk=%x rstl=%x iquad=%" VL_PRI64 "x"
                  " -> oquad=%" VL_PRI64"x owide=%x_%08x_%08x\n",
                  main_time, top->clk, top->reset_l, top->in_quad,
                  top->out_quad, top->out_wide[2], top->out_wide[1], top->out_wide[0]);
#endif
    }

    // Final model cleanup
    top->final();

    // Close trace if opened
#if VM_TRACE
    if (tfp) { tfp->close(); tfp = NULL; }
#endif

    //  Coverage analysis (since test passed)
#if VM_COVERAGE
    Verilated::mkdir("logs");
    VerilatedCov::write("logs/coverage.dat");
#endif

    // Destroy model
    delete top; top = NULL;

    // Fin
    exit(0);
}

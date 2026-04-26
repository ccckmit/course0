module mux2to1(d0, d1, sel, y);
    input d0;
    input d1;
    input sel;
    output y;

    // Use logical NOT (!) to ensure it evaluates to 0 or 1 perfectly in C
    assign y = ((!sel) & d0) | (sel & d1);
endmodule

module testbench;
    reg d0;
    reg d1;
    reg sel;
    wire y;

    mux2to1 uut(d0, d1, sel, y);

    initial begin
        d0 = 0; d1 = 1; sel = 0;
        #10;
        $display("d0=%b, d1=%b, sel=%b => y=%b", d0, d1, sel, y);
        
        d0 = 0; d1 = 1; sel = 1;
        #10;
        $display("d0=%b, d1=%b, sel=%b => y=%b", d0, d1, sel, y);
        
        d0 = 1; d1 = 0; sel = 0;
        #10;
        $display("d0=%b, d1=%b, sel=%b => y=%b", d0, d1, sel, y);
        
        d0 = 1; d1 = 0; sel = 1;
        #10;
        $display("d0=%b, d1=%b, sel=%b => y=%b", d0, d1, sel, y);
    end
endmodule

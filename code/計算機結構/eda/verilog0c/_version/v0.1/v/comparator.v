module comparator(a, b, eq, neq);
    input a;
    input b;
    output eq;
    output neq;

    // a ^ b -> 不相等時為 1
    // !(a ^ b) -> 相等時為 1
    assign eq = !(a ^ b);
    assign neq = a ^ b;
endmodule

module testbench;
    reg a;
    reg b;
    wire eq;
    wire neq;

    comparator uut(a, b, eq, neq);

    initial begin
        a = 0; b = 0;
        #10;
        $display("a=%b, b=%b => eq=%b, neq=%b", a, b, eq, neq);
        
        a = 0; b = 1;
        #10;
        $display("a=%b, b=%b => eq=%b, neq=%b", a, b, eq, neq);
        
        a = 1; b = 0;
        #10;
        $display("a=%b, b=%b => eq=%b, neq=%b", a, b, eq, neq);
        
        a = 1; b = 1;
        #10;
        $display("a=%b, b=%b => eq=%b, neq=%b", a, b, eq, neq);
    end
endmodule

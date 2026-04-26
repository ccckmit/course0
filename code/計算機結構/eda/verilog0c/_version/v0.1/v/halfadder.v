module half_adder(a, b, sum, cout);
    input a;
    input b;
    output sum;
    output cout;

    assign sum = a ^ b;
    assign cout = a & b;
endmodule

module testbench;
    reg a;
    reg b;
    wire sum;
    wire cout;

    half_adder uut(a, b, sum, cout);

    initial begin
        a = 0; b = 0;
        #10;
        $display("a=%b, b=%b => sum=%b, cout=%b", a, b, sum, cout);
        a = 0; b = 1;
        #10;
        $display("a=%b, b=%b => sum=%b, cout=%b", a, b, sum, cout);
        a = 1; b = 0;
        #10;
        $display("a=%b, b=%b => sum=%b, cout=%b", a, b, sum, cout);
        a = 1; b = 1;
        #10;
        $display("a=%b, b=%b => sum=%b, cout=%b", a, b, sum, cout);
    end
endmodule
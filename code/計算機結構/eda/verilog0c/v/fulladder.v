module full_adder(a, b, cin, sum, cout);
    input a;
    input b;
    input cin;
    output sum;
    output cout;

    assign sum = a ^ b ^ cin;
    assign cout = (a & b) | (cin & (a ^ b));
endmodule

module testbench;
    reg a;
    reg b;
    reg cin;
    wire sum;
    wire cout;

    full_adder uut(a, b, cin, sum, cout);

    initial begin
        a = 0; b = 0; cin = 0;
        #10;
        $display("a=%b, b=%b, cin=%b => sum=%b, cout=%b", a, b, cin, sum, cout);

        a = 0; b = 0; cin = 1;
        #10;
        $display("a=%b, b=%b, cin=%b => sum=%b, cout=%b", a, b, cin, sum, cout);

        a = 0; b = 1; cin = 0;
        #10;
        $display("a=%b, b=%b, cin=%b => sum=%b, cout=%b", a, b, cin, sum, cout);

        a = 0; b = 1; cin = 1;
        #10;
        $display("a=%b, b=%b, cin=%b => sum=%b, cout=%b", a, b, cin, sum, cout);

        a = 1; b = 0; cin = 0;
        #10;
        $display("a=%b, b=%b, cin=%b => sum=%b, cout=%b", a, b, cin, sum, cout);

        a = 1; b = 0; cin = 1;
        #10;
        $display("a=%b, b=%b, cin=%b => sum=%b, cout=%b", a, b, cin, sum, cout);

        a = 1; b = 1; cin = 0;
        #10;
        $display("a=%b, b=%b, cin=%b => sum=%b, cout=%b", a, b, cin, sum, cout);

        a = 1; b = 1; cin = 1;
        #10;
        $display("a=%b, b=%b, cin=%b => sum=%b, cout=%b", a, b, cin, sum, cout);
    end
endmodule

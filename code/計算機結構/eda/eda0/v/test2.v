module test2(input a, b, output z);
    // z = (a & b) | (a & ~b) ---> simplifies to a
    assign z = (a & b) | (a & ~b);
endmodule

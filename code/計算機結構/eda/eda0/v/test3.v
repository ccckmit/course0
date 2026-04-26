module test3(input a, b, output out1, out2);
    // out1 = (a | a) & b ---> simplifies to a & b
    assign out1 = (a | a) & b;
    
    // out2 = a ^ a ---> simplifies to False (0)
    assign out2 = a ^ a;
endmodule

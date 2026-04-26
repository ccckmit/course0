module test1(input a, b, c, output x, y);
    // x = (~a & b & c) | (~a & b & ~c) ---> simplifies to ~a & b
    assign x = (~a & b & c) | (~a & b & ~c);
    
    // y = (~b & ~c) | (a & ~b) | (a & c)
    assign y = (~b & ~c) | (a & ~b) | (a & c);
endmodule

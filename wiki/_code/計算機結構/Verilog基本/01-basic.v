// Verilog 基礎語法範例
// 展示模組、訊號、資料類型

// 1. Hello World - 簡單模組
module hello_world;
    initial begin
        $display("Hello, World!");
    end
endmodule

// 2. 基本邏輯閘
module and_gate (
    input a,
    input b,
    output y
);
    assign y = a & b;
endmodule

module or_gate (
    input a,
    input b,
    output y
);
    assign y = a | b;
endmodule

module not_gate (
    input a,
    output y
);
    assign y = ~a;
endmodule

module nand_gate (
    input a,
    input b,
    output y
);
    assign y = ~(a & b);
endmodule

// 3. 多位元訊號
module bus_example (
    input [7:0] a,      // 8 位元輸入
    input [7:0] b,
    output [7:0] sum,  // 8 位元輸出
    output carry       // 進位輸出
);
    assign {carry, sum} = a + b;
endmodule

// 4. 向量選擇
module vector_slice (
    input [31:0] data_in,
    output [7:0] byte0,
    output [7:0] byte1,
    output [7:0] byte2,
    output [7:0] byte3,
    output [15:0] lower_half,
    output [15:0] upper_half
);
    assign byte0 = data_in[7:0];
    assign byte1 = data_in[15:8];
    assign byte2 = data_in[23:16];
    assign byte3 = data_in[31:24];
    assign lower_half = data_in[15:0];
    assign upper_half = data_in[31:16];
endmodule

// 5. 多工器
module mux2to1 (
    input a,
    input b,
    input sel,
    output y
);
    assign y = sel ? b : a;
endmodule

module mux4to1 (
    input [3:0] data,
    input [1:0] sel,
    output y
);
    assign y = data[sel];
endmodule

// 6. 解多工器
module demux1to4 (
    input data,
    input [1:0] sel,
    output [3:0] out
);
    assign out = 1 << sel == data ? 1'b1 : 1'b0;
    // 或者
    // assign out[sel] = data;
endmodule

// 7. 編碼器與解碼器
module encoder8to3 (
    input [7:0] data_in,
    output [2:0] data_out,
    output valid
);
    assign valid = |data_in;
    always @(*) begin
        data_out = 0;
        casex (data_in)
            8'b1xxxxxxx: data_out = 3'd0;
            8'b01xxxxxx: data_out = 3'd1;
            8'b001xxxxx: data_out = 3'd2;
            8'b0001xxxx: data_out = 3'd3;
            8'b00001xxx: data_out = 3'd4;
            8'b000001xx: data_out = 3'd5;
            8'b0000001x: data_out = 3'd6;
            8'b00000001: data_out = 3'd7;
        endcase
    end
endmodule

module decoder3to8 (
    input [2:0] data_in,
    input enable,
    output [7:0] data_out
);
    assign data_out = enable ? (1 << data_in) : 8'b0;
endmodule

// 8. 比較器
module comparator (
    input [7:0] a,
    input [7:0] b,
    output equal,
    output greater,
    output less
);
    assign equal = (a == b);
    assign greater = (a > b);
    assign less = (a < b);
endmodule

// 9. 加法器
module half_adder (
    input a,
    input b,
    output sum,
    output carry
);
    assign sum = a ^ b;
    assign carry = a & b;
endmodule

module full_adder (
    input a,
    input b,
    input cin,
    output sum,
    output cout
);
    assign sum = a ^ b ^ cin;
    assign cout = (a & b) | (a & cin) | (b & cin);
endmodule

module adder8 (
    input [7:0] a,
    input [7:0] b,
    input cin,
    output [7:0] sum,
    output cout
);
    wire [7:0] temp_carry;
    
    full_adder fa0(a[0], b[0], cin, sum[0], temp_carry[0]);
    full_adder fa1(a[1], b[1], temp_carry[0], sum[1], temp_carry[1]);
    full_adder fa2(a[2], b[2], temp_carry[1], sum[2], temp_carry[2]);
    full_adder fa3(a[3], b[3], temp_carry[2], sum[3], temp_carry[3]);
    full_adder fa4(a[4], b[4], temp_carry[3], sum[4], temp_carry[4]);
    full_adder fa5(a[5], b[5], temp_carry[4], sum[5], temp_carry[5]);
    full_adder fa6(a[6], b[6], temp_carry[5], sum[6], temp_carry[6]);
    full_adder fa7(a[7], b[7], temp_carry[6], sum[7], cout);
endmodule

// 10. 測試平台範例
module testbench;
    reg [7:0] a, b;
    wire [7:0] sum;
    wire cout;
    
    // 實例化待測模組
    adder8 uut (.a(a), .b(b), .cin(1'b0), .sum(sum), .cout(cout));
    
    initial begin
        $display("Time    a       b       sum     cout");
        $monitor("%t    %b    %b    %b    %b", $time, a, b, sum, cout);
        
        a = 8'd0; b = 8'd0;
        #10 a = 8'd1; b = 8'd1;
        #10 a = 8'd10; b = 8'd20;
        #10 a = 8'd255; b = 8'd1;
        #10 $finish;
    end
endmodule

module multiplier(clk, rst, ready, op1, op2, res, done);
    // Init port
    parameter SIZE = 32;
    input clk, rst;
    input ready;
    input[SIZE-1:0] op1, op2;
    output reg [SIZE-1:0] res;
    output reg done;


always @(posedge clk, posedge rst)
begin
    if (rst == 1'b1) begin
        done <= 1'b0;
    end
    else begin
        if (ready == 1'b1) begin
            res <= op1 + op2;
            done <= 1'b1;
        end
        else begin
            res <= res;
            done <= 1'b0;
        end
    end
end


endmodule
module FrontendLogic(
    input logic eq,
    input logic [31:0] prev, prev_pc, vs,
    output logic miss,
    output logic [31:0] rpc
);
    logic [5:0] opcode, funct;
    assign opcode = prev[31:26];
    assign funct = prev[5:0];

    always_comb
    case (opcode)
        6'b000000: if (funct == 6'b001000) begin  // jr, always retake
            miss = 1;
            rpc = vs;
        end else begin
            miss = 0;
        end
        6'b000100: if (!eq) begin  // beq
            miss = 1;
            rpc = prev_pc + 4;
        end else begin
            miss = 0;  // `verilator does not affect by this, but vivado does.
        end
        6'b000101: if (eq) begin  // bne
            miss = 1;
            rpc = prev_pc + 4;
        end else begin
            miss = 0;  // the same as above
        end
        default: miss = 0;
    endcase

    logic _unused_ok = &{1'b0, prev, 1'b0};
endmodule
`include "cache.vh"
`include "CacheController.vh"

/**
 * the controller
 */
module CacheController #(
    TAG_WIDTH = `CACHE_T,
    SET_WIDTH = `CACHE_S,
    LINE_WIDTH = `CACHE_B,
    KEY_WIDTH = $clog2(`CACHE_E),
    _COUNT_MAX = 2**(LINE_WIDTH - 2)
) (
    input logic clk, reset, en,

    /**
     * interface with MIPS CPU.
     * addr has been parsed into {tag, idx, offset}.
     */
    input logic write_en,
    input logic [TAG_WIDTH - 1:0] tag,
    input logic [SET_WIDTH - 1:0] idx,
    input logic [LINE_WIDTH - 1:0] offset,
    input logic [31:0] data,
    output logic hit,
    output logic [31:0] out,

    /**
     * interface with cache set.
     *
     * write_data: data for cache write.
     */
    output logic by_tag,
    output logic [TAG_WIDTH - 1:0] target_tag,
    output logic [KEY_WIDTH - 1:0] target_key,
    output logic [LINE_WIDTH - 1:0] index,
    output logic [2:0] ctrl,
    output logic [31:0] write_data, set_tick,
    output logic [TAG_WIDTH - 1:0] set_tag,
    input logic line_hit, line_dirty,
    input logic [31:0] line_out,
    input logic [TAG_WIDTH - 1:0] line_tag,
    input logic [KEY_WIDTH - 1:0] line_key,

    /**
     * interface with memory.
     */
    output logic mwrite_en,
    output logic [31:0] maddr, mdata,
    input logic [31:0] mout
);
    logic count_reset;
    logic [31:0] now, count;
    Counter #(
        .START_COUNT(1)
    ) timer(
        .clk(clk), .reset(reset), .en(en), .out(now)
    );
    Counter counter(
        .clk(clk), .reset(count_reset || reset), .en(1),
        .out(count)
    );

    logic [1:0] state;
    CacheControllerFSM #(
        .KEY_WIDTH(KEY_WIDTH),
        .COUNT_MAX(_COUNT_MAX)
    ) fsm(
        .clk(clk), .reset(reset), .en(en),
        .state(state), .count(count),
        .line_hit(line_hit),
        .line_dirty(line_dirty)
    );

    logic normal;
    assign normal = state[1];
    assign hit = normal && line_hit;
    assign out = line_out;
    assign by_tag = normal;
    assign target_tag = tag;
    assign target_key = line_key;
    assign set_tick = now;
    assign set_tag = tag;
    assign mdata = line_out;

    logic [LINE_WIDTH - 1:0] pos;
    assign pos = count[LINE_WIDTH - 1:0] << 2;
    assign index = normal ? offset : pos;

    /**
     * dataflow & control signals for each state.
     */
    always_comb begin
        {ctrl, mwrite_en, maddr, write_data, count_reset} = 0;
        if (en) begin
            case (state)
                `WRITE: begin
                    ctrl = 3'b000;
                    mwrite_en = 1;
                    maddr = {line_tag, idx, pos};
                    if (count == _COUNT_MAX - 1)
                        count_reset = 1;
                end
                `FETCH: begin
                    if (count == _COUNT_MAX - 1)
                        ctrl = 3'b011;
                    else
                        ctrl = 3'b001;
                    maddr = {tag, idx, pos};
                    write_data = mout;
                end
                default: begin
                    ctrl = {2'b10, write_en};
                    write_data = data;
                    count_reset = !line_hit;
                end
            endcase
        end
    end
endmodule
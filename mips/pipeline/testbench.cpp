// #define NDEBUG

#include <random>
#include <algorithm>

#include "verilated.h"
#include "testbench.h"

static Device dev;
static std::vector<ITest*> test_list;

std::vector<ITest*> *p_test_list = &test_list;
ITest *current_test = nullptr;

using namespace std;

u32 randi() {
    static random_device rd;
    static mt19937 gen(rd());
    return gen();
}


//
// TESTS
//

BEGIN(1)
    dev.reset();
    assert(dev.pc0() == 0);
    for (int i = 0; i < 32; i++)
        assert(dev[i] == 0);
END(1, "reset")

BEGIN(2)
    dev.resize_imem(9);
    dev.reset();

    // dev.enable_print();
    for (int i = 0; i < 4; i++) {
        dev.run();

        assert(dev.pc0() == 8 * i);
        assert(dev.instr0() == 0);
    }
END(2, "double fetch")

BEGIN(5)
    dev.resize_imem(8);

    auto add = RTYPE(ADD, $v0, $s0, $s1, 0);
    dev.imem[0] = dev.imem[2] = add;
    dev.reset();

    // dev.enable_print();
    dev.run();
    assert(dev.pc0() == 0);
    assert(dev.instr0() == add);

    dev.run();
    assert(dev.pc0() == 8);
    assert(dev.instr0() == add);
END(5, "nop1")

BEGIN(4)
    dev.resize_imem(8);

    auto add = RTYPE(ADD, $v0, $s0, $s1, 0);
    dev.imem[0] = dev.imem[1] = add;
    dev.reset();

    // dev.enable_print();
    dev.run();
    assert(dev.pc0() == 0);
    assert(dev.instr0() == add);

    dev.run();
    assert(dev.pc0() == 4);
    assert(dev.instr0() == add);
END(4, "nop2")

BEGIN(3)
    dev.resize_imem(32);

    auto add = RTYPE(ADD, $v0, $s0, $s1, 0);
    for (int i = 0; i < 8; i++)
        dev.imem[i] = add;

    dev.reset();

    // dev.enable_print();
    for (int i = 0; i < 8; i++) {
        dev.run();
        assert(dev.pc0() == i * 4);
        assert(dev.instr0() == add);
        // printf("can_swap: %d\n", dev.dp->Datapath__DOT__frontend__DOT__select__DOT__can_swap);
    }
END(3, "normal fetch")

BEGIN(6)
    dev.resize_imem(32);
    dev.resize_dmem(32);

    for (int i = 0; i < 8; i++)
        dev.dmem[i] = i + 1;
    for (int i = 0; i < 8; i++)
        dev.imem[i] = ITYPE(LW, $0, $t0 + i, i * 4);

    // dev.enable_print();
    dev.reset();

    dev.run(11);
    for (int i = 0; i < 8; i++)
        assert(dev[$t0 + i] == i + 1);
END(6, "lw sequence")

BEGIN(7)
    dev.resize_imem(32);
    dev.resize_dmem(32);

    for (int i = 0; i < 16; i++)
        dev.dmem[i] = i * 4;
    for (int i = 0; i < 8; i++)
        dev.imem[i] = ITYPE(LW, $t0 + i, $t0 + i + 1, 4);

    // dev.enable_print();
    dev.reset();
    dev.run(18);

    for (int i = 0; i < 9; i++)
        assert(dev[$t0 + i] == dev.dmem[i]);
END(7, "lw sequence 2")

BEGIN(8)
    dev.dmem = {233, 666};
    dev.imem = {
        NOP,
        JTYPE(JMP, 4),
        ITYPE(LW, $0, $v0, 0),
        JTYPE(JMP, 5),
        ITYPE(LW, $0, $v0, 4)
    };
    dev.imem.resize(32);

    // dev.enable_print();
    dev.reset();
    dev.run(6);
    assert(dev[$v0] == 666);
END(8, "jmp in buf")

BEGIN(9)
    dev.dmem = {233, 666};
    dev.imem = {
        NOP,
        JTYPE(JAL, 4),
        ITYPE(LW, $0, $v0, 0),
        JTYPE(JMP, 5),
        ITYPE(LW, $0, $v0, 4)
    };
    dev.imem.resize(32);
    // dev.enable_print();
    dev.reset();
    dev.run(6);
    assert(dev[$ra] == 4);
    assert(dev[$v0] == 666);
END(9, "jal in buf")

BEGIN(10)
    int n = 128;

    dev.dmem.clear();
    dev.imem.clear();
    for (int i = 0; i < n; i++)
        dev.dmem.push_back(randi());
    for (int i = 0; i < n; i++) {
        dev.imem.push_back(ITYPE(LW, $0, $v1, 4 * i));
        dev.imem.push_back(RTYPE(ADD, $v0, $v1, $v0, 0));
    }
    dev.imem.resize(2 * n + 16);

    // dev.enable_print();
    dev.reset();
    dev.run(3);

    int sum = 0;
    for (int i = 0; i < n; i++) {
        dev.run(2);
        assert(dev[$v1] == dev.dmem[i]);

        dev.run(1);
        sum += dev.dmem[i];
        assert(dev[$v0] == sum);
    }
END(10, "add")

BEGIN(11)
    int n = 128;

    dev.dmem.clear();
    dev.imem.clear();
    for (int i = 0; i < n; i++)
        dev.dmem.push_back(randi());
    for (int i = 0; i < n; i++) {
        dev.imem.push_back(ITYPE(LW, $0, $v1, 4 * i));
        dev.imem.push_back(RTYPE(SUB, $v0, $v1, $v0, 0));
    }
    dev.imem.resize(2 * n + 16);

    // dev.enable_print();
    dev.reset();
    dev.run(3);

    int sum = 0;
    for (int i = 0; i < n; i++) {
        dev.run(2);
        assert(dev[$v1] == dev.dmem[i]);

        dev.run(1);
        sum -= dev.dmem[i];
        assert(dev[$v0] == sum);
    }
END(11, "sub")

BEGIN(12)
    int n = 16;

    dev.dmem.clear();
    dev.imem.clear();
    for (int i = 0; i < n; i++)
        dev.dmem.push_back(randi());
    for (int i = 0; i < n; i++) {
        dev.imem.push_back(ITYPE(LW, $0, $v1, 4 * i));
        dev.imem.push_back(RTYPE(OR, $v0, $v1, $v0, 0));
    }
    dev.imem.resize(2 * n + 16);

    // dev.enable_print();
    dev.reset();
    dev.run(3);

    int sum = 0;
    for (int i = 0; i < n; i++) {
        dev.run(2);
        assert(dev[$v1] == dev.dmem[i]);

        dev.run(1);
        sum |= dev.dmem[i];
        assert(dev[$v0] == sum);
    }
END(12, "or")

BEGIN(13)
    int n = 128;

    dev.dmem.clear();
    dev.imem.clear();
    for (int i = 0; i < n; i++)
        dev.dmem.push_back(randi());
    for (int i = 0; i < n; i++) {
        dev.imem.push_back(ITYPE(LW, $0, $v1, 4 * i));
        dev.imem.push_back(RTYPE(XOR, $v0, $v1, $v0, 0));
    }
    dev.imem.resize(2 * n + 16);

    // dev.enable_print();
    dev.reset();
    dev.run(3);

    int sum = 0;
    for (int i = 0; i < n; i++) {
        dev.run(2);
        assert(dev[$v1] == dev.dmem[i]);

        dev.run(1);
        sum ^= dev.dmem[i];
        assert(dev[$v0] == sum);
    }
END(13, "xor")

BEGIN(14)
    int n = 128;

    dev.dmem.clear();
    dev.imem.clear();
    for (int i = 0; i < n; i++)
        dev.dmem.push_back(randi());
    for (int i = 0; i < n; i++) {
        dev.imem.push_back(ITYPE(LW, $0, $v1, 4 * i));
        dev.imem.push_back(RTYPE(NOR, $v0, $v1, $v0, 0));
    }
    dev.imem.resize(2 * n + 16);

    // dev.enable_print();
    dev.reset();
    dev.run(3);

    int sum = 0;
    for (int i = 0; i < n; i++) {
        dev.run(2);
        assert(dev[$v1] == dev.dmem[i]);

        dev.run(1);
        sum = ~(sum | dev.dmem[i]);
        assert(dev[$v0] == sum);
    }
END(14, "nor")


//
// MAIN
//

typedef void handler_t(int);

void unix_error(const char *msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

handler_t *Signal(int signum, handler_t *handler) {
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
        unix_error("Signal error");
    return (old_action.sa_handler);
}

void abort_handler(int) {
    if (current_test)
        fprintf(stderr, "\033[31mERR!\033[0m abort in \"%s\"\n", current_test->name);
}

void exit_handler() {
    abort_handler(0);
}

int main(int argc, char *argv[]) {
    Verilated::commandArgs(argc, argv);

    Signal(SIGABRT, abort_handler);
    atexit(exit_handler);

    for (auto t : test_list)
        t->run();

    return 0;
}
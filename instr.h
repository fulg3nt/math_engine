#ifndef INSTR_H
#define INSTR_H

#include <cstddef>

namespace MathEngine
{

enum class OpCode {
    PUSH_CONST,
    PUSH_NUM,
    PUSH_VAR,
    CALL_FUNC,
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    NEG
};

struct Instruction {
    OpCode op;
    union {
        size_t index;
        double number;
    };

    explicit Instruction(OpCode op) : op(op), index(0) {}
    Instruction(OpCode op, size_t index) : op(op), index(index) {}
    Instruction(OpCode op, double number) : op(op), number(number) {}
};

} // namespace MathEngine

#endif // INSTR_H

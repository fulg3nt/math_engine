#ifndef DISASM_H
#define DISASM_H

#include "eval.h"
#include "instr.h"
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace MathEngine
{

inline const char *opName(OpCode op)
{
    switch (op) {
    case OpCode::PUSH_CONST:
        return "PUSH_CONST";
    case OpCode::PUSH_NUM:
        return "PUSH_NUM";
    case OpCode::PUSH_VAR:
        return "PUSH_VAR";
    case OpCode::CALL_FUNC:
        return "CALL_FUNC";
    case OpCode::ADD:
        return "ADD";
    case OpCode::SUB:
        return "SUB";
    case OpCode::MUL:
        return "MUL";
    case OpCode::DIV:
        return "DIV";
    case OpCode::POW:
        return "POW";
    case OpCode::NEG:
        return "NEG";
    }
    return "???";
}

inline void disassemble(const std::vector<Instruction> &code, const std::vector<std::string> &vars = {},
                        std::ostream &out = std::cout)
{
    for (size_t pc = 0; pc < code.size(); pc++) {
        const Instruction &instr = code[pc];
        out << std::setw(4) << std::right << pc << ": " << std::left << std::setw(11) << opName(instr.op);

        switch (instr.op) {
        case OpCode::PUSH_NUM:
            out << instr.number;
            break;
        case OpCode::PUSH_CONST:
            out << instr.index << " -> '" << Evaluator::globalConstants[instr.index].name
                << "' = " << Evaluator::globalConstants[instr.index].value;
            break;
        case OpCode::CALL_FUNC:
            out << instr.index << " -> '" << Evaluator::globalFunctions[instr.index].name << "'";
            break;
        case OpCode::PUSH_VAR:
            if (instr.index < vars.size()) {
                out << instr.index << " -> '" << vars[instr.index] << "'";
            } else {
                out << instr.index;
            }
            break;
        default:
            break;
        }
        out << '\n';
    }
}

} // namespace MathEngine

#endif // DISASM_H
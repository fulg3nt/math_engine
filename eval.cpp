#include "eval.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstring>

namespace MathEngine
{
using std::optional;

const ConstantEntry Evaluator::globalConstants[] = {
    {"pi", M_PI}, {"e", M_E}
};

const FunctionEntry Evaluator::globalFunctions[] = {
    {"sin", std::sin},
    {"cos", std::cos},
    {"tg", std::tan},
    {"ctg", [](double x) { return 1 / std::tan(x); }},
    {"sqrt", std::sqrt},
    {"ln", std::log},
    {"lg", std::log10},
    {"log", [](double base, double x) { return std::log(x) / std::log(base); }},
    {"arctg", std::atan},
    {"arcctg", [](double x) { return std::atan2(1.0, x); }},
    {"arcsin", std::asin},
    {"arccos", std::acos},
    {"floor", std::floor},
    {"ceil", std::ceil},
    {"abs", std::fabs}
};

optional<size_t> Evaluator::getConstantIndex(const char *name)
{
    for (size_t i = 0; i < std::size(globalConstants); i++) {
        if (!std::strcmp(name, globalConstants[i].name)) return i;
    }
    return std::nullopt;
}

optional<size_t> Evaluator::getFunctionIndex(const char *name)
{
    for (size_t i = 0; i < std::size(globalFunctions); i++) {
        if (!std::strcmp(name, globalFunctions[i].name)) return i;
    }
    return std::nullopt;
}

class Stack
{
  private:
    double *sp, *base;

  public:
    Stack(double *base) : base(base), sp(base) {}

    void push(double val) { *sp++ = val; }
    double pop() { return *--sp; }
    double &top() { return *(sp - 1); }
    bool empty() const { return sp == base; }
};

double Evaluator::eval(const std::vector<Instruction> &instrs,
                       const double *vars)
{
    double buf[1024];
    Stack stack(buf);

    for (const Instruction &instr : instrs) {
        switch (instr.op) {
            case OpCode::PUSH_NUM: stack.push(instr.number); break;
            case OpCode::PUSH_VAR: stack.push(vars[instr.index]); break;
            case OpCode::PUSH_CONST:
                stack.push(globalConstants[instr.index].value);
                break;

            case OpCode::CALL_FUNC: {
                const FunctionEntry &ent = globalFunctions[instr.index];

                switch (ent.argCount) {
                    case 1: stack.top() = ent.call1(stack.top()); break;
                    case 2: {
                        double arg2 = stack.pop();
                        stack.top() = ent.call2(stack.top(), arg2);
                        break;
                    }
                }
                break;
            }

#define BIN_OP(op)                                                             \
    {                                                                          \
        double b = stack.pop();                                                \
        stack.top() op## = b;                                                  \
        break;                                                                 \
    }

            case OpCode::ADD: BIN_OP(+)
            case OpCode::SUB: BIN_OP(-)
            case OpCode::MUL: BIN_OP(*)
            case OpCode::DIV: BIN_OP(/)

            case OpCode::POW: {
                double b = stack.pop();
                stack.top() = std::pow(stack.top(), b);
                break;
            }

            case OpCode::NEG: stack.top() = -stack.top(); break;
        }
    }

    return stack.top();
}

} // namespace MathEngine
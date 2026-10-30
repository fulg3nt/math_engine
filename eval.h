#ifndef EVAL_H
#define EVAL_H

#include "instr.h"
#include <optional>
#include <vector>

namespace MathEngine
{

struct ConstantEntry {
    const char *name;
    double value;
};

struct FunctionEntry {
    const char *name;
    size_t argCount;

    union {
        double (*call1)(double);
        double (*call2)(double, double);
    };

    FunctionEntry(const char *n, double (*f)(double)) : name(n), argCount(1), call1(f) {}
    FunctionEntry(const char *n, double (*f)(double, double)) : name(n), argCount(2), call2(f) {}
};

class Evaluator
{
  public:
    static const ConstantEntry globalConstants[];
    static const FunctionEntry globalFunctions[];

    static std::optional<size_t> getConstantIndex(const char *name);
    static std::optional<size_t> getFunctionIndex(const char *name);

    static double eval(const std::vector<Instruction> &instrs, const double *vars);
};

} // namespace MathEngine

#endif // EVAL_H
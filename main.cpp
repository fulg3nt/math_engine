#include "disasm.h"
#include "eval.h"
#include "input_error.h"
#include "parser.h"
#include <iostream>

int main()
{
    bool showBytecode = false;

    while (true) {
        std::cout << ">>> ";
        std::string src;

        if (!std::getline(std::cin, src)) {
            break;
        }

        if (src == ":bc") {
            showBytecode = !showBytecode;
            std::cout << "bytecode output: " << (showBytecode ? "on" : "off") << '\n';
            continue;
        }

        try {
            auto program = MathEngine::Parser::compile(src, {});
            if (showBytecode) {
                std::cout << '\n';
                MathEngine::disassemble(program);
                std::cout << '\n';
            }
            std::cout << MathEngine::Evaluator::eval(program, NULL) << '\n';
        } catch (MathEngine::InputError &e) {
            if (auto range = e.range()) {
                std::cout << "    ";
                for (size_t i = 0; i < range->start; i++) {
                    std::cout << ' ';
                }
                for (size_t i = 0; i < range->length; i++) {
                    std::cout << '^';
                }
                std::cout << '\n';
            }
            std::cout << e.what() << '\n';
        }
    }
    return 0;
}
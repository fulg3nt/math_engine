# MathEngine

Компилятор математических выражений в байткод со стековой ВМ.

Модуль (лексер, парсер, компиляция в байткод и его исполнение) разработан мной как часть командного проекта "графический калькулятор". `main.cpp` в этом репозитории - отдельная CLI-обвязка для демонстрации модуля вне остального проекта.

## Как это работает

Выражение проходит три стадии:
1. Lexer: разбивает строку на токены (числа, идентификаторы, операторы, скобки).
2. Parser: рекурсивный спуск по грамматике ниже, строит сразу линейный байткод (`std::vector<Instruction>`), минуя AST.
3. Evaluator: стековая виртуальная машина, исполняет байткод за один проход.

Грамматика (EBNF):
```
<equation> ::= <expression> EQUALS <expression> END
<expression> ::= <term> { (PLUS | MINUS) <term> }
<term> ::= <factor> { (STAR | SLASH) <factor> }
<factor> ::= ( PLUS | MINUS ) <factor> | <primary> [ CARET <factor> ]
<primary> ::= NUMBER |
              IDENTIFIER [ L_PAR <arg-list> R_PAR | L_BRK <arg-list> R_BRK ] |
              L_PAR <expression> R_PAR | L_BRK <expression> R_BRK
<arg-list> ::= <expression> { COMMA <expression> }
```

Скобки `(...)` и `[...]` работают одинаково, но требуют соответствия типа открывающей и закрывающей скобки.

## Встроенные функции и константы

Функции: `sin cos tg ctg sqrt ln lg log(base, x) arctg arcctg arcsin arccos floor ceil abs`.
Константы: `pi`, `e`.

## Сборка

Используется GNU Make совместно с g++ под MSYS2 UCRT64.
```
make
```

## Пример работы

```
>>> sin
    ^^^
function 'sin' must be called with '(...)' or '[...]'
>>> sin(pi/2
            ^
expected ')'
>>> sin(pi/2)
1
>>> log[e, e ^ 123]
123
>>> arctg(inf) - pi/2
0
```

Ошибки парсинга указывают точное место в строке (`^^^`).

## Байткод

Команда `:bc` в REPL включает вывод дизассемблированного байткода перед результатом:

```
>>> :bc
bytecode output: on
>>> arctg(inf) - pi / 2

   0: PUSH_NUM   inf
   1: CALL_FUNC  8 -> 'arctg'
   2: PUSH_CONST 0 -> 'pi' = 3.14159
   3: PUSH_NUM   2
   4: DIV        
   5: SUB        

0
```

## Статус и ограничения

- Грамматика и код для уравнений вида `... = ...` подготовлены (`parseEquation`, токен `=`), но сейчас не включены, `main.cpp` вызывает только `parseExpression`.
- Переменные поддерживаются на уровне движка (`PUSH_VAR`), но текущий `main.cpp` эту возможность не использует.

## Структура

- `lexer.h/.cpp` — токенизация
- `parser.h/.cpp` — разбор выражений, компиляция в байткод
- `instr.h` — формат инструкции байткода
- `eval.h/.cpp` — стековая ВМ, таблицы функций/констант
- `input_error.h` — ошибки парсинга с привязкой к позиции в строке
- `disasm.h` — дизассемблер байткода (для отладки/демонстрации)
- `main.cpp` — REPL

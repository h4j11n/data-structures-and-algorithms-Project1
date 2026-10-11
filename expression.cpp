#include "expression.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <string>

static void printEvaluationStacks(const Stack<char>& operatorStack,
                                  const Stack<int>& numberStack,
                                  std::ostream& trace) {
    trace << "    运算符栈:";
    printStack(operatorStack, trace);
    trace << "    运算数栈:";
    printStack(numberStack, trace);
}

char compare(char top, char incoming) {
    const int OPERATOR_COUNT = 9;
    static const char operators[OPERATOR_COUNT] = {
        '+', '-', '*', '/', '(', ')', '#', '^', '~'
    };

    // 行是栈顶，列是输入，顺序与 operators 一致；~ 表示一元负号。
    // ^ 对 ^ 为 <，实现右结合；当前语法只允许在栈顶为 # 或 ( 时读入 ~。
    static const char relations[OPERATOR_COUNT][OPERATOR_COUNT] = {
        {'>', '>', '<', '<', '<', '>', '>', '<', 'x'}, // +
        {'>', '>', '<', '<', '<', '>', '>', '<', 'x'}, // -
        {'>', '>', '>', '>', '<', '>', '>', '<', 'x'}, // *
        {'>', '>', '>', '>', '<', '>', '>', '<', 'x'}, // /
        {'<', '<', '<', '<', '<', '=', 'x', '<', '<'}, // (
        {'>', '>', '>', '>', 'x', '>', '>', '>', 'x'}, // )
        {'<', '<', '<', '<', '<', 'x', '=', '<', '<'}, // #
        {'>', '>', '>', '>', '<', '>', '>', '<', 'x'}, // ^
        {'>', '>', '>', '>', '<', '>', '>', '<', 'x'}  // ~
    };

    int row = -1;
    int column = -1;
    for (int i = 0; i < OPERATOR_COUNT; i++) {
        if (operators[i] == top) {
            row = i;
        }
        if (operators[i] == incoming) {
            column = i;
        }
    }
    if (row == -1 || column == -1) {
        return 'x';
    }
    return relations[row][column];
}

bool calculate(int left, char op, int right, int& result) {
    switch (op) {
        case '+':
            result = left + right;
            return true;
        case '-':
            result = left - right;
            return true;
        case '*':
            result = left * right;
            return true;
        case '/':
            if (right == 0) {
                return false;
            }
            result = left / right; // 整数除法舍去小数部分，向零截断。
            return true;
        case '^': {
            if (right < 0) {
                return false;
            }
            double value = std::pow(left, right);
            // pow 返回浮点数，转为 int 前要拒绝非有限值和超出范围的结果。
            if (!std::isfinite(value)
                || value < std::numeric_limits<int>::min()
                || value > std::numeric_limits<int>::max()) {
                return false;
            }
            result = static_cast<int>(value);
            return true;
        }
        default:
            return false;
    }
}

bool getNumber(const std::string& expression, int& position, int& result) {
    int value = 0;
    bool found = false;
    while (position < static_cast<int>(expression.size())
           && expression[position] >= '0' && expression[position] <= '9') {
        // 连续数字构成一个整数；循环结束后不再额外推进下标。
        value = value * 10 + (expression[position] - '0');
        position++;
        found = true;
    }
    result = value;
    return found;
}

bool evaluateExpression(const std::string& expression, int& result) {
    return evaluateExpression(expression, result, std::cout);
}

bool evaluateExpression(const std::string& expression, int& result, std::ostream& trace) {
    Stack<int> numberStack;
    Stack<char> operatorStack;
    if (!push(operatorStack, '#')) {
        return false;
    }

    std::string input = expression;
    input += '#';
    int position = 0;
    // true：等待数字、左括号或合法的一元负号；false：等待运算符、右括号或结束。
    bool expectOperand = true;

    while (!isEmpty(operatorStack)) {
        int number;
        if (getNumber(input, position, number)) {
            // 数字前必须有允许的分隔，拒绝 1 2 和 (2)3 等缺少运算符的输入。
            if (!expectOperand || !push(numberStack, number)) {
                return false;
            }
            expectOperand = false;
            trace << "运算数" << number << "入栈:\n";
            printEvaluationStacks(operatorStack, numberStack, trace);
            continue;
        }
        if (input[position] == ' ') {
            position++;
            continue;
        }

        char incoming = input[position];
        // 只接受程序追加的末尾 #，避免输入中间的 # 提前结束计算。
        if (incoming == '#' && position != static_cast<int>(input.size()) - 1) {
            return false;
        }
        if (expectOperand) {
            if (incoming == '-') {
                // 忽略空格后，一元负号只能位于开头或左括号后；3*-2 需写成 3*(-2)。
                int previous = position - 1;
                while (previous >= 0 && input[previous] == ' ') {
                    previous--;
                }
                if (previous >= 0 && input[previous] != '(') {
                    return false;
                }
                incoming = '~';
            } else if (incoming != '(') {
                // 在等待操作数时遇到右括号或终止符，说明空括号或缺少操作数。
                return false;
            }
        } else if (incoming != '+' && incoming != '-' && incoming != '*'
                   && incoming != '/' && incoming != '^'
                   && incoming != ')' && incoming != '#') {
            return false;
        }

        char top;
        if (!peek(operatorStack, top)) {
            return false;
        }
        char relation = compare(top, incoming);
        if (relation == '<') {
            if (!push(operatorStack, incoming)) {
                return false;
            }
            position++;
            expectOperand = true;
            trace << "运算符" << incoming << "入栈:\n";
            printEvaluationStacks(operatorStack, numberStack, trace);
            continue;
        }
        if (relation == '>') {
            char op;
            if (!pop(operatorStack, op)) {
                return false;
            }

            int value;
            if (op == '~') {
                int operand;
                if (!pop(numberStack, operand)) {
                    return false;
                }
                // 最小 int 取负超出 int 范围；取负只需要一个操作数。
                if (operand == std::numeric_limits<int>::min()) {
                    return false;
                }
                value = -operand;
            } else {
                int right, left;
                // 右操作数后入栈、先弹出，减法与除法不能交换左右顺序。
                if (!pop(numberStack, right) || !pop(numberStack, left)
                    || !calculate(left, op, right, value)) {
                    return false;
                }
            }
            if (!push(numberStack, value)) {
                return false;
            }

            trace << "当前输入" << incoming << ',';
            if (op == '~') {
                trace << "执行一元取负，结果为 " << value << ":\n";
            } else {
                trace << "计算" << op << ":\n";
            }
            printEvaluationStacks(operatorStack, numberStack, trace);
            // 不消耗当前输入：它还要与新的栈顶比较，可能触发连续计算。
            continue;
        }
        if (relation == '=') {
            char matched;
            if (!pop(operatorStack, matched)) {
                return false;
            }
            if (matched == '(') {
                position++; // 括号配对只消除括号，不取出运算数。
                continue;
            }
            if (matched == '#') {
                // 必须恰好剩一个数字，再将其带回 result。
                if (numberStack.top != 0) {
                    return false;
                }
                return pop(numberStack, result);
            }
        }
        return false;
    }
    return false;
}

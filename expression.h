#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <iostream>
#include <string>

const int STACK_CAPACITY = 100;

// top 是栈顶下标；-1 表示空栈。int 栈存运算数，char 栈存运算符。
template <typename T>
struct Stack {
    int top = -1;
    T data[STACK_CAPACITY];
};

// 模板实现放在头文件中，使用处才能根据具体类型生成函数。
template <typename T>
bool isEmpty(const Stack<T>& stack) {
    return stack.top == -1;
}

template <typename T>
bool push(Stack<T>& stack, T value) {
    if (stack.top == STACK_CAPACITY - 1) {
        return false;
    }

    stack.top++;
    stack.data[stack.top] = value;
    return true;
}

template <typename T>
bool pop(Stack<T>& stack, T& value) {
    if (isEmpty(stack)) {
        return false;
    }

    value = stack.data[stack.top];
    stack.top--;
    return true;
}

template <typename T>
bool peek(const Stack<T>& stack, T& value) {
    if (isEmpty(stack)) {
        return false;
    }

    value = stack.data[stack.top];
    return true;
}

template <typename T>
void clear(Stack<T>& stack) {
    stack.top = -1;
}

template <typename T>
void printStack(const Stack<T>& stack, std::ostream& output = std::cout) {
    if (isEmpty(stack)) {
        output << "空";
    }
    for (int i = 0; i <= stack.top; i++) {
        if (i > 0) {
            output << ' ';
        }
        output << stack.data[i];
    }
    output << '\n';
}

// 比较栈顶与当前输入，返回 <、>、= 或 x（不合法）。
char compare(char top, char incoming);
// 二元运算；一元负号在 evaluateExpression 中单独处理。
bool calculate(int left, char op, int right, int& result);
// 从 position 读取连续数字，结束后 position 指向数字后的第一个字符。
bool getNumber(const std::string& expression, int& position, int& result);

// 要求显式乘号；一元负号仅允许在开头或左括号后，指数必须非负。
// result 仅在返回 true 时有效。
bool evaluateExpression(const std::string& expression, int& result);
// 可把过程写入字符串流，供图形界面显示；原接口仍向终端输出。
bool evaluateExpression(const std::string& expression, int& result, std::ostream& trace);

#endif

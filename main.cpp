#include "expression.h"
#include "polynomial.h"

#include <iostream>
#include <limits>
#include <string>

void recoverInput() {
    if (!std::cin.eof()) {
        // clear 恢复读取状态，ignore 丢弃本行剩余内容；两者用途不同。
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

bool readInteger(const char* prompt, int& value) {
    std::cout << prompt;
    if (std::cin >> value) {
        return true;
    }

    if (!std::cin.eof()) {
        std::cout << "输入无效，请输入整数。\n";
    }
    recoverInput();
    return false;
}

// 菜单负责提示和失败处理，实际读入及建链表统一交给 inputPolynomial。
bool readPolynomial(const char* name, Polynomial& polynomial) {
    std::cout << "\n输入多项式 " << name << "：\n";
    int length;
    if (!readInteger("项数（零多项式输入 0）：", length)) {
        return false;
    }
    if (length < 0) {
        std::cout << "项数不能为负数。\n";
        recoverInput();
        return false;
    }

    polynomial = initPolynomial(length);
    if (length > 0) {
        std::cout << "按指数降序输入每项的系数和指数，系数非零、指数非负。\n";
        std::cout << "例如 2x^3+5x：项数为 2，接着输入 2 3 5 1。\n";
    }

    if (!inputPolynomial(polynomial)) {
        if (!std::cin.eof()) {
            std::cout << "输入无效：系数和指数须为整数，系数非零、指数非负且严格降序。"
                      << "本次计算取消。\n";
        }
        recoverInput();
        // 输入可能只完成一部分，失败时也要释放已创建的节点和头节点。
        deletePolynomial(polynomial);
        return false;
    }

    std::cout << name << " = ";
    printPolynomial(polynomial);
    return true;
}

void runPolynomialCalculator() {
    while (true) {
        std::cout << "\n多项式计算\n";
        std::cout << "1. 加法（a+b）\n2. 减法（a-b）\n3. 乘法（a*b）\n";
        std::cout << "4. 求导\n5. 代入求值\n0. 返回主菜单\n";

        int choice;
        if (!readInteger("请选择：", choice)) {
            if (std::cin.eof()) {
                return;
            }
            continue;
        }
        if (choice == 0) {
            return; // 结束子菜单函数，回到主菜单。
        }
        if (choice < 1 || choice > 5) {
            std::cout << "请输入 0 到 5。\n";
            continue;
        }

        // 未用到的多项式保持空指针，结束时可以统一调用删除函数。
        Polynomial a{0, nullptr};
        Polynomial b{0, nullptr};
        Polynomial result{0, nullptr};
        if (!readPolynomial("a", a)) {
            if (std::cin.eof()) {
                return;
            }
            continue;
        }
        // 只有加、减、乘需要第二个多项式；&& 保证其他选项不会读取 b。
        if (choice <= 3 && !readPolynomial("b", b)) {
            deletePolynomial(a);
            if (std::cin.eof()) {
                return;
            }
            continue;
        }

        switch (choice) {
            case 1:
                result = addPolynomial(a, b);
                std::cout << "a+b = ";
                break;
            case 2:
                result = subtractPolynomial(a, b);
                std::cout << "a-b = ";
                break;
            case 3:
                result = multiplyPolynomial(a, b);
                std::cout << "a*b = ";
                break;
            case 4:
                result = differentiatePolynomial(a);
                std::cout << "a 的导函数 = ";
                break;
            case 5: {
                double x;
                std::cout << "请输入 x：";
                if (std::cin >> x) {
                    std::cout << "a(" << x << ") = "
                              << evaluatePolynomial(a, x) << '\n';
                } else {
                    if (!std::cin.eof()) {
                        std::cout << "x 必须是数字，本次计算取消。\n";
                    }
                    recoverInput();
                }
                break;
            }
        }

        // 求值返回数字，其余运算返回新链表；无论输入是否结束都先释放资源。
        if (choice != 5) {
            printPolynomial(result);
        }
        deletePolynomial(a);
        deletePolynomial(b);
        deletePolynomial(result);
        if (std::cin.eof()) {
            return;
        }
    }
}

void runExpressionCalculator() {
    // >> 读取菜单数字后留下换行，先清除菜单这一行再使用 getline。
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "请输入表达式（整数、+ - * / ^ 和括号，无需输入 #）：\n";
    std::cout << "一元负号只允许在开头或左括号后，例如 -3+2、3*(-2)。"
              << "指数必须是非负整数。\n";
    std::cout << "说明：运算符栈中的 ~ 表示一元负号（取负），- 表示减法。\n";
    std::string expression;
    if (!std::getline(std::cin, expression)) {
        return;
    }

    int result;
    if (evaluateExpression(expression, result)) {
        std::cout << "结果：" << result << '\n';
    } else {
        std::cout << "求值失败，请检查表达式语法、括号、除数、指数或数值范围。\n";
    }
}

int main() {
    while (true) {
        std::cout << "\n1. 多项式计算\n";
        std::cout << "2. 表达式求值\n";
        std::cout << "0. 退出\n";

        int choice;
        if (!readInteger("请选择：", choice)) {
            if (std::cin.eof()) {
                return 0;
            }
            continue;
        }

        switch (choice) {
            case 1:
                runPolynomialCalculator();
                break;
            case 2:
                runExpressionCalculator();
                break;
            case 0:
                std::cout << "已退出。\n";
                return 0;
            default:
                std::cout << "请输入 0、1 或 2。\n";
                break;
        }
        // break 只离开 switch，随后循环重新显示菜单；EOF 则结束程序。
        if (std::cin.eof()) {
            return 0;
        }
    }
}

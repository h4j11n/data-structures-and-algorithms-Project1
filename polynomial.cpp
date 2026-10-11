#include "polynomial.h"

#include <cmath>
#include <iostream>

Polynomial initPolynomial(int length) {
    Polynomial polynomial;
    polynomial.head = new Term{0, 0, nullptr};
    polynomial.length = length;
    return polynomial;
}

bool inputPolynomial(Polynomial& polynomial) {
    return inputPolynomial(polynomial, std::cin);
}

bool inputPolynomial(Polynomial& polynomial, std::istream& input) {
    if (polynomial.head == nullptr || polynomial.head->next != nullptr
        || polynomial.length < 0) {
        return false;
    }

    // 保留预计项数，随后逐项统计实际节点数，保证输入中途失败时 length 仍准确。
    int expectedLength = polynomial.length;
    polynomial.length = 0;
    Term* tail = polynomial.head;
    for (int i = 0; i < expectedLength; i++) {
        int coef, exp;
        if (!(input >> coef >> exp)) {
            return false;
        }
        // 严格降序同时排除了重复指数；第一项不与头节点的指数比较。
        if (coef == 0 || exp < 0 || (i > 0 && exp >= tail->exp)) {
            return false;
        }

        tail->next = new Term{exp, coef, nullptr};
        tail = tail->next;
        polynomial.length++;
    }
    return true;
}

void printPolynomial(const Polynomial& polynomial) {
    printPolynomial(polynomial, std::cout);
}

void printPolynomial(const Polynomial& polynomial, std::ostream& output) {
    bool first = true;
    output << std::noshowpos;
    for (const Term* current = polynomial.head->next;
         current != nullptr; current = current->next) {
        if (current->coef == 0) {
            continue;
        }

        // 首项省略正号，后续项显示符号；系数的绝对值单独输出。
        if (current->coef < 0) {
            output << '-';
        } else if (!first) {
            output << '+';
        }

        // 先扩大整数范围，避免最小 int 取绝对值时溢出。
        long long magnitude = current->coef;
        if (magnitude < 0) {
            magnitude = -magnitude;
        }
        if (current->exp == 0 || magnitude != 1) {
            output << magnitude;
        }
        if (current->exp != 0) {
            output << 'x';
            if (current->exp != 1) {
                output << '^' << current->exp;
            }
        }
        first = false;
    }

    if (first) {
        output << 0;
    }
    output << '\n';
}

Polynomial addPolynomial(const Polynomial& a, const Polynomial& b) {
    Polynomial result = initPolynomial(0);
    Term* tail = result.head;
    const Term* term1 = a.head->next;
    const Term* term2 = b.head->next;

    // 合并两条有序链表：指数不同时，只推进已经复制的那一条链。
    while (term1 != nullptr && term2 != nullptr) {
        if (term1->exp > term2->exp) {
            tail->next = new Term{term1->exp, term1->coef, nullptr};
            tail = tail->next;
            term1 = term1->next;
            result.length++;
        } else if (term1->exp < term2->exp) {
            tail->next = new Term{term2->exp, term2->coef, nullptr};
            tail = tail->next;
            term2 = term2->next;
            result.length++;
        } else {
            // 同指数项合并；系数相消时不建节点，尾指针也不移动。
            int coef = term1->coef + term2->coef;
            if (coef != 0) {
                tail->next = new Term{term1->exp, coef, nullptr};
                tail = tail->next;
                result.length++;
            }
            term1 = term1->next;
            term2 = term2->next;
        }
    }

    // 主循环结束时至少有一条链为空，另一条的剩余项直接复制。
    while (term1 != nullptr) {
        tail->next = new Term{term1->exp, term1->coef, nullptr};
        tail = tail->next;
        term1 = term1->next;
        result.length++;
    }
    while (term2 != nullptr) {
        tail->next = new Term{term2->exp, term2->coef, nullptr};
        tail = tail->next;
        term2 = term2->next;
        result.length++;
    }
    return result;
}

Polynomial subtractPolynomial(const Polynomial& a, const Polynomial& b) {
    Polynomial result = initPolynomial(0);
    Term* tail = result.head;
    const Term* term1 = a.head->next;
    const Term* term2 = b.head->next;

    // 与加法采用相同的合并过程，但来自 b 的项要改变符号。
    while (term1 != nullptr && term2 != nullptr) {
        if (term1->exp > term2->exp) {
            tail->next = new Term{term1->exp, term1->coef, nullptr};
            tail = tail->next;
            term1 = term1->next;
            result.length++;
        } else if (term1->exp < term2->exp) {
            tail->next = new Term{term2->exp, -term2->coef, nullptr};
            tail = tail->next;
            term2 = term2->next;
            result.length++;
        } else {
            int coef = term1->coef - term2->coef;
            if (coef != 0) {
                tail->next = new Term{term1->exp, coef, nullptr};
                tail = tail->next;
                result.length++;
            }
            term1 = term1->next;
            term2 = term2->next;
        }
    }

    while (term1 != nullptr) {
        tail->next = new Term{term1->exp, term1->coef, nullptr};
        tail = tail->next;
        term1 = term1->next;
        result.length++;
    }
    while (term2 != nullptr) {
        tail->next = new Term{term2->exp, -term2->coef, nullptr};
        tail = tail->next;
        term2 = term2->next;
        result.length++;
    }
    return result;
}

Polynomial differentiatePolynomial(const Polynomial& polynomial) {
    Polynomial result = initPolynomial(0);
    Term* tail = result.head;
    for (const Term* current = polynomial.head->next;
         current != nullptr; current = current->next) {
        // 常数的导数为零，不保留节点；其余项的指数都减一，原顺序不变。
        if (current->exp == 0) {
            continue;
        }
        tail->next = new Term{current->exp - 1,
                              current->coef * current->exp, nullptr};
        tail = tail->next;
        result.length++;
    }
    return result;
}

double evaluatePolynomial(const Polynomial& polynomial, double x) {
    double result = 0;
    for (const Term* current = polynomial.head->next;
         current != nullptr; current = current->next) {
        result += current->coef * std::pow(x, current->exp);
    }
    return result;
}

Polynomial multiplyPolynomial(const Polynomial& a, const Polynomial& b) {
    Polynomial result = initPolynomial(0);
    for (const Term* term1 = a.head->next;
         term1 != nullptr; term1 = term1->next) {
        // 用 a 的一项乘整个 b，得到仍然按指数降序排列的临时多项式。
        Polynomial partial = initPolynomial(0);
        Term* tail = partial.head;
        for (const Term* term2 = b.head->next;
             term2 != nullptr; term2 = term2->next) {
            int exp = term1->exp + term2->exp;
            int coef = term1->coef * term2->coef;
            tail->next = new Term{exp, coef, nullptr};
            tail = tail->next;
            partial.length++;
        }

        // 加法创建独立节点并合并同类项；先保存新结果，再释放旧链表。
        Polynomial sum = addPolynomial(result, partial);
        deletePolynomial(result);
        deletePolynomial(partial);
        result = sum;
    }
    return result;
}

void deletePolynomial(Polynomial& polynomial) {
    Term* current = polynomial.head;
    while (current != nullptr) {
        // 当前节点删除后不能再访问，必须提前保存下一节点的地址。
        Term* next = current->next;
        delete current;
        current = next;
    }
    polynomial.head = nullptr;
    polynomial.length = 0;
}

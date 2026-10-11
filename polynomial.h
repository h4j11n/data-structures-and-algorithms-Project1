#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <istream>
#include <ostream>

struct Term {
    int exp;
    int coef;
    Term* next;
};

// 带头节点的单链表：有效项从 head->next 开始，按指数严格降序排列。
struct Polynomial {
    int length;
    Term* head;
};

// 初始化时 length 表示待输入项数；inputPolynomial 完成后表示实际项数。
Polynomial initPolynomial(int length);
// 仅向刚初始化的空链表读入；失败时可能留下部分节点，由调用者释放。
bool inputPolynomial(Polynomial& polynomial);
bool inputPolynomial(Polynomial& polynomial, std::istream& input);
void printPolynomial(const Polynomial& polynomial);
void printPolynomial(const Polynomial& polynomial, std::ostream& output);

// 运算结果使用独立的新节点；调用者使用完后应调用 deletePolynomial。
Polynomial addPolynomial(const Polynomial& a, const Polynomial& b);
Polynomial subtractPolynomial(const Polynomial& a, const Polynomial& b);
Polynomial differentiatePolynomial(const Polynomial& polynomial);
double evaluatePolynomial(const Polynomial& polynomial, double x);
Polynomial multiplyPolynomial(const Polynomial& a, const Polynomial& b);

// 连同头节点一起释放；销毁后若要再次使用，需要重新初始化。
void deletePolynomial(Polynomial& polynomial);

#endif

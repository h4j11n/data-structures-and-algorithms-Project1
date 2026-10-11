#include "polynomial_input.h"

#include <cctype>
#include <limits>
#include <sstream>
#include <string>

QString normalizeMathSymbols(QString text) {
    text.replace(QChar(0x00D7), '*');
    text.replace(QChar(0x00F7), '/');
    text.replace(QChar(0x2212), '-');
    text.replace(QChar(0xFF08), '(');
    text.replace(QChar(0xFF09), ')');
    text.replace('X', 'x');
    return text;
}

static void skipSpaces(const std::string& text, std::size_t& position) {
    while (position < text.size()
           && std::isspace(static_cast<unsigned char>(text[position]))) {
        position++;
    }
}

static bool readUnsigned(const std::string& text, std::size_t& position,
                         long long maximum, long long& value, bool& found) {
    value = 0;
    found = false;
    while (position < text.size() && text[position] >= '0' && text[position] <= '9') {
        int digit = text[position] - '0';
        if (value > (maximum - digit) / 10) {
            return false;
        }
        value = value * 10 + digit;
        position++;
        found = true;
    }
    return true;
}

bool polynomialFromText(const QString& source, Polynomial& result, QString& error) {
    std::string text = normalizeMathSymbols(source).trimmed().toStdString();
    if (text.empty()) {
        error = "请先输入多项式，例如 2x^3+5x。";
        return false;
    }

    Polynomial normalized = initPolynomial(0);
    auto fail = [&](const QString& message) {
        error = message;
        deletePolynomial(normalized);
        return false;
    };
    std::size_t position = 0;
    bool first = true;
    while (position < text.size()) {
        skipSpaces(text, position);
        int sign = 1;
        if (position < text.size() && (text[position] == '+' || text[position] == '-')) {
            sign = text[position] == '-' ? -1 : 1;
            position++;
            skipSpaces(text, position);
        } else if (!first) {
            return fail("多项式各项之间需要 + 或 -。例如 2x^3+5x。");
        }

        long long magnitude;
        bool hasCoefficient;
        if (!readUnsigned(text, position,
                          static_cast<long long>(std::numeric_limits<int>::max()) + 1,
                          magnitude, hasCoefficient)) {
            return fail("系数过大，请使用较小的整数。");
        }
        skipSpaces(text, position);
        if (position < text.size() && text[position] == '*') {
            if (!hasCoefficient) {
                return fail("乘号前需要系数，例如 2*x^3。");
            }
            position++;
            skipSpaces(text, position);
            if (position == text.size() || text[position] != 'x') {
                return fail("多项式输入框只接收单项的加减，例如 2x^3+5x。");
            }
        }

        int exponent = 0;
        if (position < text.size() && text[position] == 'x') {
            position++;
            if (!hasCoefficient) {
                magnitude = 1;
            }
            exponent = 1;
            skipSpaces(text, position);
            if (position < text.size() && text[position] == '^') {
                position++;
                skipSpaces(text, position);
                long long value;
                bool hasExponent;
                if (!readUnsigned(text, position, std::numeric_limits<int>::max(),
                                  value, hasExponent) || !hasExponent) {
                    return fail("指数必须是非负整数，例如 x^3。");
                }
                exponent = static_cast<int>(value);
            }
        } else if (!hasCoefficient) {
            return fail("多项式格式有误。支持 2x^3、-x、4 等单项。");
        }

        long long signedCoefficient = sign * magnitude;
        if (signedCoefficient < std::numeric_limits<int>::min()
            || signedCoefficient > std::numeric_limits<int>::max()) {
            return fail("系数过大，请使用较小的整数。");
        }
        int coefficient = static_cast<int>(signedCoefficient);
        if (coefficient != 0) {
            // 用原 inputPolynomial 构造单项，不在界面中另写建链表逻辑。
            std::ostringstream data;
            data << coefficient << ' ' << exponent;
            std::istringstream input(data.str());
            Polynomial term = initPolynomial(1);
            if (!inputPolynomial(term, input)) {
                deletePolynomial(term);
                return fail("无法读取当前项，请检查系数和指数。");
            }
            for (const Term* current = normalized.head->next;
                 current != nullptr; current = current->next) {
                if (current->exp == exponent) {
                    long long sum = static_cast<long long>(current->coef) + coefficient;
                    if (sum < std::numeric_limits<int>::min()
                        || sum > std::numeric_limits<int>::max()) {
                        deletePolynomial(term);
                        return fail("合并同类项后的系数过大。");
                    }
                    break;
                }
            }
            Polynomial sum = addPolynomial(normalized, term);
            deletePolynomial(normalized);
            deletePolynomial(term);
            normalized = sum;
        }
        skipSpaces(text, position);
        if (position < text.size() && text[position] != '+' && text[position] != '-') {
            return fail("多项式格式有误。请输入单项的加减，例如 x^2+2x+1。");
        }
        first = false;
    }
    result = normalized;
    return true;
}

QString polynomialText(const Polynomial& polynomial) {
    std::ostringstream output;
    printPolynomial(polynomial, output);
    return QString::fromStdString(output.str()).trimmed();
}

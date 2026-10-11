#ifndef POLYNOMIAL_INPUT_H
#define POLYNOMIAL_INPUT_H

#include "polynomial.h"
#include <QString>

QString normalizeMathSymbols(QString text);
// 把自然书写的各项转换为原输入函数接收的数据，再用原加法合并同类项。
// 成功返回独立链表；失败时释放中间节点。
bool polynomialFromText(const QString& text, Polynomial& result, QString& error);
QString polynomialText(const Polynomial& polynomial);

#endif

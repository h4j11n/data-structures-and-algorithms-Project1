#ifndef CALCULATOR_WINDOW_H
#define CALCULATOR_WINDOW_H

#include <QWidget>
#include <QString>

class QEvent;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QStackedWidget;

// 所有交互都由按钮事件触发；这里不需要自己写输入循环。
class CalculatorWindow : public QWidget {
public:
    CalculatorWindow();

protected:
    bool eventFilter(QObject* object, QEvent* event) override;

private:
    bool polynomialMode = false;
    bool expressionError = false;
    bool polynomialError = false;
    QString expressionResult = "0";
    QString polynomialResult = "0";
    QLineEdit* activeInput = nullptr;
    QLineEdit* expressionInput = nullptr;
    QLineEdit* polynomialA = nullptr;
    QLineEdit* polynomialB = nullptr;
    QLineEdit* xInput = nullptr;
    QLineEdit* resultDisplay = nullptr;
    QPlainTextEdit* outputBox = nullptr;
    QStackedWidget* inputPages = nullptr;
    int polynomialOperation = 0;
    QPushButton* operationButtons[5]{};
    QPushButton* modeButton = nullptr;
    QPushButton* keyButtons[21]{};
    QLabel* modeTitle = nullptr;
    QLabel* inputHint = nullptr;

    void buildUi();
    void updateMode();
    void updatePolynomialOperation();
    void updateKeypad();
    void handleKey(const QString& token);
    void calculate();
    void calculateExpression();
    void calculatePolynomial();
    void showHelp();
    void setResult(const QString& value, bool error = false);
};

#endif

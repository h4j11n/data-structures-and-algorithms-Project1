#include "calculator_window.h"
#include "expression.h"
#include "polynomial_input.h"

#include <QButtonGroup>
#include <QDoubleValidator>
#include <QEvent>
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QShortcut>
#include <QStackedWidget>
#include <QStyle>
#include <QVBoxLayout>
#include <cmath>
#include <sstream>

CalculatorWindow::CalculatorWindow() {
    setObjectName("calculatorWindow");
    setWindowTitle("多项式与表达式计算器");
    resize(1000, 800);
    setMinimumSize(900, 740);
    buildUi();
    setStyleSheet(R"(
        QWidget { color: #253342; font-family: "PingFang SC"; font-size: 14px; }
        QWidget#calculatorWindow { background: #f4f6f8; }
        QFrame#leftPanel { background: #edf1f5; border-radius: 20px; }
        QFrame#calculatorPanel { background: #ffffff; border: 1px solid #e2e7ec; border-radius: 20px; }
        QLabel[role="title"] { font-size: 23px; font-weight: 600; }
        QLabel[role="caption"] { color: #86929f; font-size: 12px; }
        QLabel[role="hint"] { color: #85919e; font-size: 12px; }
        QLineEdit { background: #f8fafc; border: 1px solid #dce3e9; border-radius: 9px;
                    padding: 7px 12px; font-size: 17px; selection-background-color: #267f72; }
        QLineEdit:focus { border: 1px solid #267f72; background: #ffffff; }
        QLineEdit:disabled { color: #aeb7bf; background: #f1f3f5; }
        QLineEdit#resultDisplay { background: transparent; border: none; padding: 0;
                                 font-size: 32px; font-weight: 600; color: #1b5149; }
        QLineEdit#resultDisplay[error="true"] { color: #b34f4f; font-size: 24px; }
        QPlainTextEdit { background: transparent; border: none; padding: 0;
                         font-family: "Menlo"; font-size: 12px; color: #526172; }
        QPushButton { background: #ffffff; border: 1px solid #e0e5ea; border-radius: 11px; }
        QPushButton:hover { background: #f0f5f6; border-color: #cad9d7; }
        QPushButton:pressed { background: #e2edeb; }
        QPushButton[keyType="number"] { font-size: 23px; }
        QPushButton[keyType="operator"] { background: #edf5f3; color: #267f72; font-size: 23px; }
        QPushButton[keyType="utility"] { background: #f0f3f6; color: #667482; font-size: 17px; }
        QPushButton:disabled { background: #f6f7f9; color: #c1c9d0; border-color: #edf0f3; }
        QPushButton#equalsButton { background: #267f72; color: #ffffff; border: none; font-size: 26px; }
        QPushButton#equalsButton:hover { background: #216f64; }
        QPushButton#equalsButton:pressed { background: #18584f; }
        QPushButton#modeButton { background: #253342; color: #ffffff; border: none; padding: 7px 14px; }
        QPushButton#clearLog { background: transparent; color: #718090; border: none; font-size: 12px; }
        QPushButton[role="operation"] { border-radius: 7px; padding: 4px 6px; font-size: 13px; }
        QPushButton[role="operation"]:checked { background: #267f72; color: #ffffff; border-color: #267f72; }
        QScrollBar:vertical { background: transparent; width: 7px; margin: 0; }
        QScrollBar::handle:vertical { background: #c5cfd8; border-radius: 3px; min-height: 24px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    )");
    updateMode();
}

void CalculatorWindow::buildUi() {
    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(18);

    auto* leftPanel = new QFrame(this);
    leftPanel->setObjectName("leftPanel");
    leftPanel->setMinimumWidth(300);
    auto* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(22, 24, 22, 22);
    leftLayout->setSpacing(14);
    auto* logHeading = new QHBoxLayout;
    auto* logTitle = new QLabel("过程与提示", leftPanel);
    logTitle->setProperty("role", "title");
    auto* clearLog = new QPushButton("清除", leftPanel);
    clearLog->setObjectName("clearLog");
    clearLog->setFocusPolicy(Qt::NoFocus);
    clearLog->setFixedSize(46, 28);
    logHeading->addWidget(logTitle);
    logHeading->addStretch();
    logHeading->addWidget(clearLog);
    leftLayout->addLayout(logHeading);
    outputBox = new QPlainTextEdit(leftPanel);
    outputBox->setObjectName("outputBox");
    outputBox->setReadOnly(true);
    outputBox->setMaximumBlockCount(2000);
    outputBox->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    leftLayout->addWidget(outputBox, 1);
    auto* logFooter = new QLabel("运算过程可滚动查看", leftPanel);
    logFooter->setProperty("role", "hint");
    leftLayout->addWidget(logFooter);
    connect(clearLog, &QPushButton::clicked, this, [this]() { showHelp(); });

    auto* rightPanel = new QFrame(this);
    rightPanel->setObjectName("calculatorPanel");
    auto* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(24, 24, 24, 24);
    rightLayout->setSpacing(12);
    auto* modeRow = new QHBoxLayout;
    modeTitle = new QLabel(rightPanel);
    modeTitle->setProperty("role", "title");
    modeButton = new QPushButton(rightPanel);
    modeButton->setObjectName("modeButton");
    modeButton->setFocusPolicy(Qt::NoFocus);
    modeRow->addWidget(modeTitle);
    modeRow->addStretch();
    modeRow->addWidget(modeButton);
    rightLayout->addLayout(modeRow);
    connect(modeButton, &QPushButton::clicked, this, [this]() {
        polynomialMode = !polynomialMode;
        updateMode();
    });

    inputPages = new QStackedWidget(rightPanel);
    inputPages->setFixedHeight(144);
    auto* expressionPage = new QWidget(inputPages);
    auto* expressionLayout = new QVBoxLayout(expressionPage);
    expressionLayout->setContentsMargins(0, 6, 0, 0);
    expressionLayout->setSpacing(8);
    auto* expressionCaption = new QLabel("表达式", expressionPage);
    expressionCaption->setProperty("role", "caption");
    expressionInput = new QLineEdit(expressionPage);
    expressionInput->setObjectName("expressionInput");
    expressionInput->setPlaceholderText("例如 ((2+3)×4+1)÷3");
    expressionInput->setFixedHeight(46);
    expressionInput->setMaxLength(4096);
    expressionLayout->addWidget(expressionCaption);
    expressionLayout->addWidget(expressionInput);
    auto* expressionHint = new QLabel("整数运算 · 括号 · 乘方 · 一元负号", expressionPage);
    expressionHint->setProperty("role", "hint");
    expressionLayout->addWidget(expressionHint);
    expressionLayout->addStretch();
    inputPages->addWidget(expressionPage);

    auto* polynomialPage = new QWidget(inputPages);
    auto* polynomialLayout = new QVBoxLayout(polynomialPage);
    polynomialLayout->setContentsMargins(0, 4, 0, 0);
    polynomialLayout->setSpacing(8);
    polynomialA = new QLineEdit(polynomialPage);
    polynomialA->setObjectName("polynomialA");
    polynomialA->setPlaceholderText("例如 2x^3+5x");
    polynomialB = new QLineEdit(polynomialPage);
    polynomialB->setObjectName("polynomialB");
    polynomialB->setPlaceholderText("例如 3x^3+4");
    for (QLineEdit* field : {polynomialA, polynomialB}) {
        field->setFixedHeight(38);
        field->setMaxLength(4096);
    }
    auto* rowA = new QHBoxLayout;
    auto* rowB = new QHBoxLayout;
    auto* labelA = new QLabel("A", polynomialPage);
    auto* labelB = new QLabel("B", polynomialPage);
    labelA->setFixedWidth(16);
    labelB->setFixedWidth(16);
    rowA->addWidget(labelA);
    rowA->addWidget(polynomialA);
    rowB->addWidget(labelB);
    rowB->addWidget(polynomialB);
    polynomialLayout->addLayout(rowA);
    polynomialLayout->addLayout(rowB);
    auto* operationRow = new QHBoxLayout;
    const QString operationLabels[5] = {"+", "−", "×", "求导", "求值"};
    const QString operationNames[5] = {"A + B", "A − B", "A × B", "A 的导函数", "A 在 x 处的值"};
    auto* operationGroup = new QButtonGroup(polynomialPage);
    operationGroup->setExclusive(true);
    operationRow->setSpacing(6);
    for (int index = 0; index < 5; index++) {
        auto* button = new QPushButton(operationLabels[index], polynomialPage);
        button->setObjectName("operation_" + QString::number(index));
        button->setProperty("role", "operation");
        button->setToolTip(operationNames[index]);
        button->setCheckable(true);
        button->setFocusPolicy(Qt::NoFocus);
        button->setFixedHeight(34);
        button->setMinimumWidth(40);
        operationGroup->addButton(button, index);
        operationButtons[index] = button;
        operationRow->addWidget(button, 1);
        // 跟随选中状态更新，鼠标、键盘和辅助功能操作都能保持同步。
        connect(button, &QPushButton::toggled, this, [this, index](bool checked) {
            if (checked) {
                polynomialOperation = index;
                updatePolynomialOperation();
            }
        });
    }
    operationRow->addSpacing(8);
    operationRow->addWidget(new QLabel("x =", polynomialPage));
    xInput = new QLineEdit("2", polynomialPage);
    xInput->setObjectName("xInput");
    xInput->setFixedSize(88, 34);
    auto* validator = new QDoubleValidator(xInput);
    validator->setLocale(QLocale::c());
    xInput->setValidator(validator);
    operationRow->addWidget(xInput);
    polynomialLayout->addLayout(operationRow);
    inputPages->addWidget(polynomialPage);
    rightLayout->addWidget(inputPages);

    auto* resultCaption = new QLabel("结果", rightPanel);
    resultCaption->setProperty("role", "caption");
    rightLayout->addWidget(resultCaption);
    resultDisplay = new QLineEdit("0", rightPanel);
    resultDisplay->setObjectName("resultDisplay");
    resultDisplay->setReadOnly(true);
    resultDisplay->setFixedHeight(52);
    resultDisplay->setAlignment(Qt::AlignRight);
    rightLayout->addWidget(resultDisplay);
    inputHint = new QLabel("可直接使用键盘输入，按回车计算", rightPanel);
    inputHint->setProperty("role", "hint");
    rightLayout->addWidget(inputHint);
    rightLayout->addStretch();

    auto* keypad = new QGridLayout;
    keypad->setHorizontalSpacing(9);
    keypad->setVerticalSpacing(9);
    const QString tokens[5][4] = {
        {"C", "⌫", "(", ")"},
        {"7", "8", "9", "÷"},
        {"4", "5", "6", "×"},
        {"1", "2", "3", "−"},
        {"0", ".", "x", "+"}
    };
    const char* names[5][4] = {
        {"clear", "backspace", "open", "close"},
        {"7", "8", "9", "divide"},
        {"4", "5", "6", "multiply"},
        {"1", "2", "3", "minus"},
        {"0", "decimal", "x", "plus"}
    };
    int index = 0;
    for (int row = 0; row < 5; row++) {
        for (int column = 0; column < 4; column++) {
            const QString token = tokens[row][column];
            auto* button = new QPushButton(token, rightPanel);
            button->setObjectName("key_" + QString::fromLatin1(names[row][column]));
            button->setProperty("keyToken", token);
            button->setProperty("keyType", row == 0 ? "utility"
                                : (column == 3 || token == "x" ? "operator" : "number"));
            button->setFocusPolicy(Qt::NoFocus);
            button->setFixedHeight(48);
            keyButtons[index++] = button;
            keypad->addWidget(button, row, column);
            // token 按值保存，按钮点击时仍然能知道自己代表哪个按键。
            connect(button, &QPushButton::clicked, this, [this, token]() { handleKey(token); });
        }
    }
    auto* powerButton = new QPushButton("^", rightPanel);
    powerButton->setObjectName("key_power");
    powerButton->setProperty("keyToken", "^");
    powerButton->setProperty("keyType", "operator");
    powerButton->setFocusPolicy(Qt::NoFocus);
    powerButton->setFixedHeight(48);
    keyButtons[20] = powerButton;
    keypad->addWidget(powerButton, 5, 0);
    connect(powerButton, &QPushButton::clicked, this, [this]() { handleKey("^"); });
    auto* equalsButton = new QPushButton("=", rightPanel);
    equalsButton->setObjectName("equalsButton");
    equalsButton->setFocusPolicy(Qt::NoFocus);
    equalsButton->setFixedHeight(48);
    keypad->addWidget(equalsButton, 5, 1, 1, 3);
    rightLayout->addLayout(keypad);
    connect(equalsButton, &QPushButton::clicked, this, [this]() { calculate(); });

    // 焦点决定数字键盘输入到表达式、A、B 还是 x，不会被按钮抢走。
    for (QLineEdit* field : {expressionInput, polynomialA, polynomialB, xInput}) {
        field->installEventFilter(this);
    }
    auto* returnKey = new QShortcut(QKeySequence(Qt::Key_Return), this);
    auto* enterKey = new QShortcut(QKeySequence(Qt::Key_Enter), this);
    connect(returnKey, &QShortcut::activated, this, [this]() { calculate(); });
    connect(enterKey, &QShortcut::activated, this, [this]() { calculate(); });
    mainLayout->addWidget(leftPanel, 4);
    mainLayout->addWidget(rightPanel, 6);
}

bool CalculatorWindow::eventFilter(QObject* object, QEvent* event) {
    if (event->type() == QEvent::FocusIn) {
        auto* field = qobject_cast<QLineEdit*>(object);
        if (field != nullptr && field->isEnabled()) {
            activeInput = field;
            updateKeypad();
        }
    }
    return QWidget::eventFilter(object, event);
}

void CalculatorWindow::updateMode() {
    inputPages->setCurrentIndex(polynomialMode ? 1 : 0);
    modeTitle->setText(polynomialMode ? "多项式计算" : "表达式计算");
    modeButton->setText(polynomialMode ? "切换到表达式" : "切换到多项式");
    activeInput = polynomialMode ? polynomialA : expressionInput;
    if (polynomialMode) {
        updatePolynomialOperation();
        setResult(polynomialResult, polynomialError);
    } else {
        setResult(expressionResult, expressionError);
    }
    showHelp();
    updateKeypad();
    activeInput->setFocus();
}

void CalculatorWindow::updatePolynomialOperation() {
    int operation = polynomialOperation;
    for (int index = 0; index < 5; index++) {
        operationButtons[index]->setChecked(index == operation);
    }
    polynomialB->setEnabled(operation <= 2);
    xInput->setEnabled(operation == 4);
    if (activeInput != nullptr && !activeInput->isEnabled()) {
        activeInput = polynomialA;
        activeInput->setFocus();
    }
    updateKeypad();
}

void CalculatorWindow::updateKeypad() {
    for (QPushButton* button : keyButtons) {
        if (button == nullptr) {
            continue;
        }
        QString token = button->property("keyToken").toString();
        bool enabled = true;
        if (!polynomialMode) {
            enabled = token != "x" && token != ".";
        } else if (activeInput == xInput) {
            enabled = token != "x" && token != "^" && token != "×"
                      && token != "÷" && token != "(" && token != ")";
        } else {
            enabled = token != "." && token != "÷" && token != "(" && token != ")";
        }
        button->setEnabled(enabled);
    }
}

void CalculatorWindow::handleKey(const QString& token) {
    if (activeInput == nullptr) {
        return;
    }
    if (token == "C") {
        if (polynomialMode) {
            polynomialA->clear();
            polynomialB->clear();
            xInput->setText("2");
            activeInput = polynomialA;
        } else {
            expressionInput->clear();
            activeInput = expressionInput;
        }
        setResult("0");
    } else if (token == "⌫") {
        activeInput->backspace();
    } else {
        activeInput->insert(token);
    }
    activeInput->setFocus();
    updateKeypad();
}

void CalculatorWindow::setResult(const QString& value, bool error) {
    if (polynomialMode) {
        polynomialResult = value;
        polynomialError = error;
    } else {
        expressionResult = value;
        expressionError = error;
    }
    resultDisplay->setText(value);
    resultDisplay->setProperty("error", error);
    resultDisplay->style()->unpolish(resultDisplay);
    resultDisplay->style()->polish(resultDisplay);
}

void CalculatorWindow::showHelp() {
    if (polynomialMode) {
        outputBox->setPlainText("直接输入多项式\n\n例如：2x^3+5x\n无需输入项数，可以合并同类项。\n\n选择加、减、乘、求导或求值，\n然后点击 =。\n\nx 为求值参数，可输入小数。\n\n点击 A、B 或 x，键盘会输入到\n对应位置。");
    } else {
        outputBox->setPlainText("输入表达式后，点击 = 或按回车。\n\n例如：((2+3)×4+1)÷3\n\n支持整数、括号和乘方。\n负数可写成 -3 或 (-3)。\n\n运算过程中：\n栈按底 → 顶显示\n~ 表示一元负号（取负）\n- 表示二元减法\n# 表示输入结束");
    }
}

void CalculatorWindow::calculate() {
    if (polynomialMode) {
        calculatePolynomial();
    } else {
        calculateExpression();
    }
    outputBox->verticalScrollBar()->setValue(0);
}

void CalculatorWindow::calculateExpression() {
    QString expression = normalizeMathSymbols(expressionInput->text()).trimmed();
    if (expression.isEmpty()) {
        setResult("请输入表达式", true);
        outputBox->setPlainText("输入为空。\n\n可以试试：(2+3)×4");
        return;
    }
    std::ostringstream trace;
    int result;
    bool success = evaluateExpression(expression.toStdString(), result, trace);
    QString details = QString::fromStdString(trace.str());
    if (success) {
        setResult(QString::number(result));
        outputBox->setPlainText("表达式：" + expressionInput->text() + "\n结果："
                               + QString::number(result) + "\n\n运算过程\n"
                               + "~ 为取负，# 为结束\n\n" + details);
    } else {
        setResult("表达式有误", true);
        outputBox->setPlainText("无法计算：" + expressionInput->text()
                               + "\n\n请检查括号、操作数和除数。\n"
                               + "负数请写成 (-2)，指数必须非负。\n\n"
                               + "停止前的运算过程\n\n" + details);
    }
}

void CalculatorWindow::calculatePolynomial() {
    Polynomial a{0, nullptr};
    Polynomial b{0, nullptr};
    Polynomial result{0, nullptr};
    auto cleanup = [&]() {
        deletePolynomial(a);
        deletePolynomial(b);
        deletePolynomial(result);
    };
    QString error;
    int operation = polynomialOperation;
    if (!polynomialFromText(polynomialA->text(), a, error)) {
        setResult("请检查多项式 A", true);
        outputBox->setPlainText("多项式 A\n\n" + error);
        cleanup();
        return;
    }
    if (operation <= 2 && !polynomialFromText(polynomialB->text(), b, error)) {
        setResult("请检查多项式 B", true);
        outputBox->setPlainText("多项式 B\n\n" + error);
        cleanup();
        return;
    }
    QString details = "A = " + polynomialText(a) + '\n';
    if (operation <= 2) {
        details += "B = " + polynomialText(b) + '\n';
    }
    QString answer;
    if (operation == 0) {
        result = addPolynomial(a, b);
    } else if (operation == 1) {
        result = subtractPolynomial(a, b);
    } else if (operation == 2) {
        result = multiplyPolynomial(a, b);
    } else if (operation == 3) {
        result = differentiatePolynomial(a);
    } else {
        bool valid;
        double x = xInput->text().toDouble(&valid);
        if (!valid || !std::isfinite(x)) {
            setResult("请输入 x 的值", true);
            outputBox->setPlainText("求值需要一个有效的 x，例如 2 或 1.5。");
            cleanup();
            return;
        }
        double value = evaluatePolynomial(a, x);
        if (!std::isfinite(value)) {
            setResult("数值过大", true);
            outputBox->setPlainText("求值结果过大，请减小 x 或多项式指数。");
            cleanup();
            return;
        }
        answer = QString::number(value, 'g', 12);
        details += "x = " + QString::number(x, 'g', 12) + '\n';
    }
    if (operation != 4) {
        answer = polynomialText(result);
    }
    setResult(answer);
    const QString operationNames[5] = {"A + B", "A − B", "A × B", "A 的导函数", "A 在 x 处的值"};
    details += "\n运算：" + operationNames[operation] + "\n\n结果：" + answer;
    outputBox->setPlainText(details);
    cleanup();
}

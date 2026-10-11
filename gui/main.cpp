#include "calculator_window.h"
#include <QApplication>
#include <QColor>
#include <QFont>
#include <QPalette>

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);
    application.setApplicationName("多项式与表达式计算器");
    application.setOrganizationName("Project1");
    application.setStyle("Fusion");
    application.setFont(QFont("PingFang SC", 13));
    // 使用明确的浅色配色，使系统深色模式下的输入框和下拉菜单也能清楚显示。
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#f4f6f8"));
    palette.setColor(QPalette::WindowText, QColor("#253342"));
    palette.setColor(QPalette::Base, QColor("#ffffff"));
    palette.setColor(QPalette::Text, QColor("#253342"));
    palette.setColor(QPalette::Button, QColor("#ffffff"));
    palette.setColor(QPalette::ButtonText, QColor("#253342"));
    palette.setColor(QPalette::Highlight, QColor("#267f72"));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    application.setPalette(palette);

    CalculatorWindow window;
    window.show();
    return application.exec();
}

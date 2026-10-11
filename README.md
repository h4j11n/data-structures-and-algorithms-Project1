# Project 1 多项式与表达式求值计算器

## 环境与依赖版本

- macOS 14.5，Apple Silicon。
- Apple clang 16.0.0；终端版 C++11，图形版 C++17。
- Qt Widgets 6.8.3。
- CMake 3.21 或以上（本机 4.4.4）。
- 可选安装工具：Python 3.9 或以上、aqtinstall 3.3.0。
- 无强制要求的编辑器插件。

## 启动入口

在项目根目录执行。

终端版入口：`main.cpp`

```sh
clang++ -std=c++11 main.cpp polynomial.cpp expression.cpp -o calculator_cli
./calculator_cli
```

图形版入口：`gui/main.cpp`

```sh
./run_gui.command
```

脚本默认使用 `$HOME/Qt/6.8.3/macos`；生成的 `build/gui/Calculator.app` 可直接双击运行。
实验报告放在 `report/` 中。

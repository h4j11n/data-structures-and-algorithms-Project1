#!/bin/bash
set -e
project_dir="$(cd "$(dirname "$0")" && pwd)"
qt_dir="${PROJECT1_QT_PATH:-$HOME/Qt/6.8.3/macos}"
build_dir="$project_dir/build/gui"

if [ ! -d "$qt_dir/lib/cmake/Qt6" ]; then
    echo "没有找到 Qt。请设置 PROJECT1_QT_PATH 为 Qt 的 macos 目录。"
    exit 1
fi
if command -v cmake >/dev/null 2>&1; then
    cmake -S "$project_dir" -B "$build_dir" -DCMAKE_PREFIX_PATH="$qt_dir"
    cmake --build "$build_dir" --parallel 2
else
    python3 -m cmake -S "$project_dir" -B "$build_dir" -DCMAKE_PREFIX_PATH="$qt_dir"
    python3 -m cmake --build "$build_dir" --parallel 2
fi
if [ -x "$qt_dir/bin/macdeployqt" ]; then
    "$qt_dir/bin/macdeployqt" "$build_dir/Calculator.app" -always-overwrite -codesign=- -verbose=0
fi
open "$build_dir/Calculator.app"

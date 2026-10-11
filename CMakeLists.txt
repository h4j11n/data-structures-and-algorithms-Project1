cmake_minimum_required(VERSION 3.21)
project(Project1Calculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.8 REQUIRED COMPONENTS Widgets)

add_library(calculator_core polynomial.cpp expression.cpp)
target_include_directories(calculator_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(calculator_cli main.cpp)
target_link_libraries(calculator_cli PRIVATE calculator_core)

add_executable(Calculator MACOSX_BUNDLE
    gui/main.cpp
    gui/calculator_window.cpp
    gui/polynomial_input.cpp
)
target_link_libraries(Calculator PRIVATE calculator_core Qt6::Widgets)
set_target_properties(Calculator PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "local.project1.calculator"
    MACOSX_BUNDLE_BUNDLE_NAME "多项式与表达式计算器"
    MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
)
if(MSVC)
    target_compile_options(calculator_core PRIVATE /utf-8)
    target_compile_options(calculator_cli PRIVATE /utf-8)
    target_compile_options(Calculator PRIVATE /utf-8)
else()
    target_compile_options(calculator_core PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(Calculator PRIVATE -Wall -Wextra -Wpedantic)
endif()

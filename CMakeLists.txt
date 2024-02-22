cmake_minimum_required(VERSION 3.27)
project(BasicPlusPlus)

set(CMAKE_CXX_STANDARD 23)

add_executable(BasicPlusPlus src/main.cpp
        src/Tokenization.hpp
        src/Tokenization.cpp
        src/Parser.cpp
        src/Parser.hpp
        src/ExpressionsStatements.hpp
        src/Interpreter.cpp
        src/Interpreter.hpp)

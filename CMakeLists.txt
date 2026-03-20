cmake_minimum_required(VERSION 3.20)
project(CZap)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(CZap main.cpp
        ASTNodes/ASTNode.cpp
        ASTNodes/ASTNode.h
        ASTNodes/Statement.cpp
        ASTNodes/Statement.h
        ASTNodes/Expression.cpp
        ASTNodes/Expression.h
        ASTNodes/VarDeclaration.cpp
        ASTNodes/VarDeclaration.h
        ASTNodes/VarReassignment.cpp
        ASTNodes/VarReassignment.h
        ASTNodes/OutputStatement.cpp
        ASTNodes/OutputStatement.h
        ASTNodes/RequestStatement.cpp
        ASTNodes/RequestStatement.h
        ASTNodes/Identifier.cpp
        ASTNodes/Identifier.h
        ASTNodes/NumberLiteral.cpp
        ASTNodes/NumberLiteral.h
        ASTNodes/StringLiteral.cpp
        ASTNodes/StringLiteral.h
        ASTNodes/BooleanLiteral.cpp
        ASTNodes/BooleanLiteral.h
        ASTNodes/BinaryExpression.cpp
        ASTNodes/BinaryExpression.h
        ASTNodes/BooleanExpression.cpp
        ASTNodes/BooleanExpression.h
        ASTNodes/FunctionCall.cpp
        ASTNodes/FunctionCall.h
        Lexer.cpp
        Lexer.h
        Tokens/Token.cpp
        Tokens/Token.h
        Environment/Environment.cpp
        Environment/Environment.h
        Helpers/Value.cpp
        Helpers/Value.h
)

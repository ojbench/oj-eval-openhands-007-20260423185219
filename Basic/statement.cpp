/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"
#include <sstream>

/* Implementation of the Statement class */

int stringToInt(std::string str);

Statement::Statement() = default;

Statement::~Statement() = default;

// RemStatement implementation
RemStatement::RemStatement() = default;
RemStatement::~RemStatement() = default;
void RemStatement::execute(EvalState &state, Program &program) {
    // REM statements do nothing
}

// LetStatement implementation
LetStatement::LetStatement(std::string var, Expression *exp) : variable(var), expression(exp) {}
LetStatement::~LetStatement() {
    delete expression;
}
void LetStatement::execute(EvalState &state, Program &program) {
    int value = expression->eval(state);
    state.setValue(variable, value);
}

// PrintStatement implementation
PrintStatement::PrintStatement(Expression *exp) : expression(exp) {}
PrintStatement::~PrintStatement() {
    delete expression;
}
void PrintStatement::execute(EvalState &state, Program &program) {
    int value = expression->eval(state);
    std::cout << value << std::endl;
}

// InputStatement implementation
InputStatement::InputStatement(std::string var) : variable(var) {}
InputStatement::~InputStatement() = default;
void InputStatement::execute(EvalState &state, Program &program) {
    std::string input;
    std::getline(std::cin, input);
    int value = stringToInt(input);
    state.setValue(variable, value);
}

// EndStatement implementation
EndStatement::EndStatement() = default;
EndStatement::~EndStatement() = default;
void EndStatement::execute(EvalState &state, Program &program) {
    throw ErrorException("END");
}

// GotoStatement implementation
GotoStatement::GotoStatement(int lineNum) : lineNumber(lineNum) {}
GotoStatement::~GotoStatement() = default;
void GotoStatement::execute(EvalState &state, Program &program) {
    throw lineNumber;
}

// IfStatement implementation
IfStatement::IfStatement(Expression *e1, std::string oper, Expression *e2, int lineNum) 
    : exp1(e1), exp2(e2), op(oper), lineNumber(lineNum) {}
IfStatement::~IfStatement() {
    delete exp1;
    delete exp2;
}
void IfStatement::execute(EvalState &state, Program &program) {
    int value1 = exp1->eval(state);
    int value2 = exp2->eval(state);
    
    bool condition = false;
    if (op == "=") {
        condition = (value1 == value2);
    } else if (op == "<") {
        condition = (value1 < value2);
    } else if (op == ">") {
        condition = (value1 > value2);
    }
    
    if (condition) {
        throw lineNumber;
    }
}

// Helper function to convert string to int
int stringToInt(std::string str) {
    if (str.empty()) return 0;
    std::istringstream iss(str);
    int value;
    iss >> value;
    if (iss.fail()) return 0;
    return value;
}

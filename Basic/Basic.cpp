/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);
void executeProgram(Program &program, EvalState &state);
void listProgram(Program &program);
Statement* parseStatement(TokenScanner &scanner);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    if (!scanner.hasMoreTokens()) return;
    
    std::string firstToken = scanner.nextToken();
    
    // Check if first token is a line number
    if (scanner.getTokenType(firstToken) == NUMBER) {
        int lineNumber = stringToInteger(firstToken);
        
        if (!scanner.hasMoreTokens()) {
            // Line number alone - remove the line
            program.removeSourceLine(lineNumber);
        } else {
            // Line number with statement - add/replace the line
            program.addSourceLine(lineNumber, line);
        }
        return;
    }
    
    // Direct commands (no line number)
    scanner.saveToken(firstToken);
    std::string command = scanner.nextToken();
    
    if (command == "RUN") {
        executeProgram(program, state);
    } else if (command == "LIST") {
        listProgram(program);
    } else if (command == "CLEAR") {
        program.clear();
        state.Clear();
    } else if (command == "QUIT") {
        exit(0);
    } else if (command == "HELP") {
        // Optional - not tested
        std::cout << "BASIC Interpreter - Commands available:" << std::endl;
        std::cout << "RUN, LIST, CLEAR, QUIT, HELP" << std::endl;
        std::cout << "Or enter statements with line numbers to build a program" << std::endl;
    } else {
        // Direct execution of statements
        scanner.saveToken(command);
        Statement *stmt = parseStatement(scanner);
        if (stmt) {
            stmt->execute(state, program);
            delete stmt;
        }
    }
}

void executeProgram(Program &program, EvalState &state) {
    int currentLine = program.getFirstLineNumber();
    
    while (currentLine != -1) {
        Statement *stmt = program.getParsedStatement(currentLine);
        bool shouldDelete = false;
        
        if (!stmt) {
            // Parse the statement if not already parsed
            std::string sourceLine = program.getSourceLine(currentLine);
            TokenScanner scanner;
            scanner.ignoreWhitespace();
            scanner.scanNumbers();
            scanner.setInput(sourceLine);
            
            // Skip line number
            scanner.nextToken();
            
            stmt = parseStatement(scanner);
            shouldDelete = true;
        }
        
        try {
            stmt->execute(state, program);
            if (shouldDelete) {
                delete stmt;
            }
            currentLine = program.getNextLineNumber(currentLine);
        } catch (int lineNumber) {
            // GOTO or IF-THEN jump
            if (shouldDelete) {
                delete stmt;
            }
            currentLine = lineNumber;
        } catch (ErrorException &ex) {
            if (shouldDelete) {
                delete stmt;
            }
            if (ex.getMessage() == "END") {
                break;
            } else {
                throw ex;
            }
        }
    }
}

void listProgram(Program &program) {
    int currentLine = program.getFirstLineNumber();
    while (currentLine != -1) {
        std::cout << program.getSourceLine(currentLine) << std::endl;
        currentLine = program.getNextLineNumber(currentLine);
    }
}

Statement* parseStatement(TokenScanner &scanner) {
    if (!scanner.hasMoreTokens()) return nullptr;
    
    std::string command = scanner.nextToken();
    
    if (command == "REM") {
        return new RemStatement();
    } else if (command == "LET") {
        std::string var = scanner.nextToken();
        if (scanner.nextToken() != "=") {
            error("SYNTAX ERROR");
        }
        Expression *exp = parseExp(scanner);
        return new LetStatement(var, exp);
    } else if (command == "PRINT") {
        Expression *exp = parseExp(scanner);
        return new PrintStatement(exp);
    } else if (command == "INPUT") {
        std::string var = scanner.nextToken();
        return new InputStatement(var);
    } else if (command == "END") {
        return new EndStatement();
    } else if (command == "GOTO") {
        std::string lineStr = scanner.nextToken();
        int lineNumber = stringToInteger(lineStr);
        return new GotoStatement(lineNumber);
    } else if (command == "IF") {
        Expression *exp1 = readE(scanner);
        std::string op = scanner.nextToken();
        Expression *exp2 = readE(scanner);
        if (scanner.nextToken() != "THEN") {
            error("SYNTAX ERROR");
        }
        std::string lineStr = scanner.nextToken();
        int lineNumber = stringToInteger(lineStr);
        return new IfStatement(exp1, op, exp2, lineNumber);
    } else {
        error("SYNTAX ERROR");
    }
    
    return nullptr;
}


/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"

Program::Program() = default;

Program::~Program() {
    clear();
}

void Program::clear() {
    // Clear parsed statements and free memory
    for (auto& pair : parsedStatements) {
        delete pair.second;
    }
    parsedStatements.clear();
    sourceLines.clear();
    lineNumbers.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    sourceLines[lineNumber] = line;
    lineNumbers.insert(lineNumber);
    
    // Remove existing parsed statement if line is being replaced
    if (parsedStatements.find(lineNumber) != parsedStatements.end()) {
        delete parsedStatements[lineNumber];
        parsedStatements.erase(lineNumber);
    }
}

void Program::removeSourceLine(int lineNumber) {
    sourceLines.erase(lineNumber);
    lineNumbers.erase(lineNumber);
    
    // Remove and free parsed statement
    if (parsedStatements.find(lineNumber) != parsedStatements.end()) {
        delete parsedStatements[lineNumber];
        parsedStatements.erase(lineNumber);
    }
}

std::string Program::getSourceLine(int lineNumber) {
    auto it = sourceLines.find(lineNumber);
    if (it != sourceLines.end()) {
        return it->second;
    }
    return "";
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    // Remove existing parsed statement if any
    if (parsedStatements.find(lineNumber) != parsedStatements.end()) {
        delete parsedStatements[lineNumber];
    }
    parsedStatements[lineNumber] = stmt;
}

Statement *Program::getParsedStatement(int lineNumber) {
    auto it = parsedStatements.find(lineNumber);
    if (it != parsedStatements.end()) {
        return it->second;
    }
    return nullptr;
}

int Program::getFirstLineNumber() {
    if (lineNumbers.empty()) {
        return -1;
    }
    return *lineNumbers.begin();
}

int Program::getNextLineNumber(int lineNumber) {
    auto it = lineNumbers.find(lineNumber);
    if (it != lineNumbers.end()) {
        ++it;
        if (it != lineNumbers.end()) {
            return *it;
        }
    }
    return -1;
}



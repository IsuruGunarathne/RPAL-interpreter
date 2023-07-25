#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <sstream>
#include <iostream>
#include <unordered_set>

#include "Token.h"

#include <unordered_set>
#include <sstream>
#include <iostream>

std::unordered_set<std::string> keywords = {
        "let", "where", "within", "aug", "fn", "in"};

std::unordered_set<std::string> operators = {
        "and", "or", "not", "gr", "ge", "ls", "le", "eq", "ne"};

std::unordered_set<std::string> booleanValues = {
        "true", "false"};

/**
 * @brief The Lexer class tokenizes the input string.
 */
class Lexer {
public:
    /**
     * @brief Constructs a Lexer object with the given input string.
     * @param input The input string to tokenize.
     */
    Lexer(const std::string &input) : input(input), currentPosition(0) {}

    /**
     * @brief Retrieves the next token from the input string.
     * @return The next token.
     */
    Token getNextToken() {
        skipWhitespace();

        if (currentPosition >= input.length()) {
            // Check if it is the last empty line or end of input
            if (currentPosition == input.length())
                return {token_type::END_OF_FILE, ""};
            else
                return {token_type::DELIMITER, ""};
        }

        char currentChar = input[currentPosition++];

        if (isalpha(currentChar)) {
            std::stringstream ss;
            ss << currentChar;
            while (currentPosition < input.length() &&
                   (isalnum(input[currentPosition]) || input[currentPosition] == '_')) {
                ss << input[currentPosition++];
            }
            std::string identifier = ss.str();

            // Check if the identifier is a keyword
            if (keywords.count(identifier) > 0) {
                return {token_type::KEYWORD, identifier};
            } else if (operators.count(identifier) > 0) {
                return {token_type::OPERATOR, identifier};
            } else if (booleanValues.count(identifier) > 0) {
                if (identifier == "true")
                    return {token_type::INTEGER, "1"};
                else
                    return {token_type::INTEGER, "0"};
            }

            return {token_type::IDENTIFIER, identifier};
        } else if (isdigit(currentChar)) {
            std::stringstream ss;
            ss << currentChar;
            while (currentPosition < input.length() && isdigit(input[currentPosition])) {
                ss << input[currentPosition++];
            }
            std::string number = ss.str();
            return {token_type::INTEGER, number};
        } else if (currentChar == '/') {
            if (currentPosition < input.length() && input[currentPosition] == '/') {
                // Skip single-line comment
                while (currentPosition < input.length() && input[currentPosition] != '\n') {
                    currentPosition++;
                }
                // Recursively call getNextToken to get the next valid token
                return getNextToken();
            } else if (isOperatorSymbol(currentChar)) {
                std::stringstream ss;
                ss << currentChar;
                while (currentPosition < input.length() && isOperatorSymbol(input[currentPosition])) {
                    ss << input[currentPosition++];
                }
                std::string op = ss.str();
                return {token_type::OPERATOR, op};
            } else {
                std::cerr << "Error: Unknown token encountered" << std::endl;
                return {token_type::END_OF_FILE, ""};
            }
        } else if (isOperatorSymbol(currentChar)) {
            std::stringstream ss;
            ss << currentChar;

            if (ss.str() == ",") {
                return {token_type::OPERATOR, ss.str()};
            }

            while (currentPosition < input.length() && isOperatorSymbol(input[currentPosition])) {
                ss << input[currentPosition++];
            }
            std::string op = ss.str();
            return {token_type::OPERATOR, op};
        } else if (currentChar == '\'' || currentChar == '"') {
            bool isSingleQuote = currentChar == '\'';

            std::stringstream ss;
//            ss << currentChar;
            while (currentPosition < input.length()) {
                currentChar = input[currentPosition++];
                if (currentChar == '\'' && isSingleQuote || currentChar == '"' && !isSingleQuote) {
//                    ss << currentChar;
                    break;
                } else if (currentChar == '\\') {
                    currentChar = input[currentPosition++];
                    switch (currentChar) {
                        case 't':
                            ss << '\t';
                            break;
                        case 'n':
                            ss << '\n';
                            break;
                        case '\\':
                            ss << '\\';
                            break;
                        case '\'':
                            ss << '\'';
                            break;
                        default:
                            ss << '\\' << currentChar;
                            break;
                    }
                } else {
                    ss << currentChar;
                }
            }
            std::string str = ss.str();
            return {token_type::STRING, str};
        } else if (currentChar == '(' || currentChar == ')') {
            std::string delimiter(1, currentChar);
            return {token_type::DELIMITER, delimiter};
        } else {
            std::cerr << "Error: Unknown token encountered" << std::endl;
            return {token_type::END_OF_FILE, ""};
        }
    }

private:
    /**
     * @brief Skips whitespace characters in the input string.
     */
    void skipWhitespace() {
        while (currentPosition < input.length() && isspace(input[currentPosition])) {
            currentPosition++;
        }
    }

    /**
     * @brief Checks if the given character is an operator symbol.
     * @param c The character to check.
     * @return True if the character is an operator symbol, false otherwise.
     */
    bool isOperatorSymbol(char c) {
        static const std::string operators = "+-*<>&.@/:=~|$!#%^_[}{?,";
        return operators.find(c) != std::string::npos;
    }

private:
    std::string input;
    size_t currentPosition;
};

#endif // LEXER_H
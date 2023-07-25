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
    Lexer(const std::string &input) : input(input), currPosition(0) {}

    /**
     * @brief Retrieves the next token from the input string.
     * @return The next token.
     */
    Token getNextToken() {
        skipWhitespace();

        if (currPosition >= input.length()) {
            // Check if it is the last empty line or end of input
            if (currPosition == input.length())
                return {type_of_token::END_OF_FILE, ""};
            else
                return {type_of_token::DELIMITER, ""};
        }

        char currentChar = input[currPosition++];

        if (isalpha(currentChar)) {
            std::stringstream ss;
            ss << currentChar;
            while (currPosition < input.length() &&
                   (isalnum(input[currPosition]) || input[currPosition] == '_')) {
                ss << input[currPosition++];
            }
            std::string identifier = ss.str();

            // Check if the identifier is a keyword
            if (keywords.count(identifier) > 0) {
                return {type_of_token::KEYWORD, identifier};
            } else if (operators.count(identifier) > 0) {
                return {type_of_token::OPERATOR, identifier};
            } else if (booleanValues.count(identifier) > 0) {
                if (identifier == "true")
                    return {type_of_token::INTEGER, "1"};
                else
                    return {type_of_token::INTEGER, "0"};
            }

            return {type_of_token::IDENTIFIER, identifier};
        } else if (isdigit(currentChar)) {
            std::stringstream ss;
            ss << currentChar;
            while (currPosition < input.length() && isdigit(input[currPosition])) {
                ss << input[currPosition++];
            }
            std::string number = ss.str();
            return {type_of_token::INTEGER, number};
        } else if (currentChar == '/') {
            if (currPosition < input.length() && input[currPosition] == '/') {
                // Skip single-line comment
                while (currPosition < input.length() && input[currPosition] != '\n') {
                    currPosition++;
                }
                // Recursively call getNextToken to get the next valid token
                return getNextToken();
            } else if (isOperatorSymbol(currentChar)) {
                std::stringstream ss;
                ss << currentChar;
                while (currPosition < input.length() && isOperatorSymbol(input[currPosition])) {
                    ss << input[currPosition++];
                }
                std::string op = ss.str();
                return {type_of_token::OPERATOR, op};
            } else {
                std::cerr << "Error: Unknown token encountered" << std::endl;
                return {type_of_token::END_OF_FILE, ""};
            }
        } else if (isOperatorSymbol(currentChar)) {
            std::stringstream ss;
            ss << currentChar;

            if (ss.str() == ",") {
                return {type_of_token::OPERATOR, ss.str()};
            }

            while (currPosition < input.length() && isOperatorSymbol(input[currPosition])) {
                ss << input[currPosition++];
            }
            std::string op = ss.str();
            return {type_of_token::OPERATOR, op};
        } else if (currentChar == '\'' || currentChar == '"') {
            bool isSingleQuote = currentChar == '\'';

            std::stringstream ss;
//            ss << currentChar;
            while (currPosition < input.length()) {
                currentChar = input[currPosition++];
                if (currentChar == '\'' && isSingleQuote || currentChar == '"' && !isSingleQuote) {
//                    ss << currentChar;
                    break;
                } else if (currentChar == '\\') {
                    currentChar = input[currPosition++];
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
            return {type_of_token::STRING, str};
        } else if (currentChar == '(' || currentChar == ')') {
            std::string delimiter(1, currentChar);
            return {type_of_token::DELIMITER, delimiter};
        } else {
            std::cerr << "Error: Unknown token encountered" << std::endl;
            return {type_of_token::END_OF_FILE, ""};
        }
    }

private:
    /**
     * @brief Skips whitespace characters in the input string.
     */
    void skipWhitespace() {
        while (currPosition < input.length() && isspace(input[currPosition])) {
            currPosition++;
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
    size_t currPosition;
};

#endif // LEXER_H
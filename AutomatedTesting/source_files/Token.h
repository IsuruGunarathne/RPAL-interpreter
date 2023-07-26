#ifndef TOKEN_H
#define TOKEN_H

#include <string>

/**
 * Types of tokens are enumerated.
 */
enum type_of_token
{
    IDENTIFIER, // A token that represents an identifier 
    INTEGER,    // A token that represents an integer
    STRING,     // A token that represents a string
    OPERATOR,   // A token that represents an operator
    DELIMITER,  // A token that represents a delimiter
    KEYWORD,    // A token that represents a keyword
    END_OF_FILE // A token that represents an end of a file
};
/**
 * A token is represented in the following structure.
 */
struct Token
{
    type_of_token type;       // The type of the token
    std::string nodeValue; // The nodeValue of the token
};

#endif // TOKEN_H
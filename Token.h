#ifndef TOKEN_H
#define TOKEN_H

#include <string>

/**
 * Enumeration of token types.
 */
enum token_type
{
    IDENTIFIER, // Represents an identifier token
    INTEGER,    // Represents an integer token
    STRING,     // Represents a string token
    OPERATOR,   // Represents an operator token
    DELIMITER,  // Represents a delimiter token
    KEYWORD,    // Represents a keyword token
    END_OF_FILE // Represents the end of file token
};

/**
 * Structure representing a token.
 */
struct Token
{
    token_type type;   // The type of the token
    std::string value; // The value of the token
};

#endif // TOKEN_H
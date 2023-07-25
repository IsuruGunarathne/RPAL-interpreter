#ifndef TOKENSSTORAGE_H
#define TOKENSSTORAGE_H

#include "Lexer.h"
#include <vector>

/**
 * 
 * The TokenStorage class is responsible for storing and managing tokens during parsing.
 * It implements the singleton pattern to ensure only one instance exists throughout the program.
 */
class TokenStorage
{
private:
    static TokenStorage instance; // Singleton instance
    std::vector<Token> tokens;    // Vector to store tokens
    int currPosition;          // Current position in the tokens vector
    Lexer *lexer;                 // Pointer to the lexer

    // Private constructor to prevent instantiation
    TokenStorage() {}

    // Private destructor to prevent deletion of the instance
    ~TokenStorage() {}

    // Private copy constructor and assignment operator to prevent copying
    TokenStorage(const TokenStorage &) = delete;
    TokenStorage &operator=(const TokenStorage &) = delete;

    /**
     * Retrieves tokens from the lexer and stores them in the tokens vector until the end of file token is encountered.
     */
    void setTokens()
    {
        Token token;
        do
        {
            token = lexer->getNextToken();
            tokens.push_back(token);
        } while (token.type != type_of_token::END_OF_FILE);
    }

public:
    /**
     * Returns the instance of the TokenStorage class.
     * @return The singleton instance of the TokenStorage class.
     */
    static TokenStorage &getInstance()
    {
        return instance;
    }

    /**
     * Sets the lexer and initializes the tokens vector.
     * @param lexer The lexer object to set.
     */
    void setLexer(Lexer &lexer)
    {
        this->lexer = &lexer;
        setTokens();
        currPosition = 0;
    }

    /**
     * Returns a reference to the current token at the top of the tokens vector.
     * @return A reference to the current token.
     */
    Token &top()
    {
        return tokens[currPosition];
    }

    /**
     * Removes and returns the current token at the top of the tokens vector.
     * @return The current token.
     */
    Token &pop()
    {
        return tokens[currPosition++];
    }

    /**
     * Resets the current position to the beginning of the tokens vector.
     */
    void reset()
    {
        currPosition = 0;
    }

    /**
     * Clears the tokens vector and sets the lexer to nullptr.
     * This method should be called when the TokenStorage instance is no longer needed.
     */
    static void clearInstance()
    {
        instance.lexer = 0;
        instance.tokens.clear();
    }
};

TokenStorage TokenStorage::instance; // Initialize the static instance

#endif // TOKENSSTORAGE_H
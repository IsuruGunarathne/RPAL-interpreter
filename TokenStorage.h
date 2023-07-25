#ifndef TOKENSSTORAGE_H
#define TOKENSSTORAGE_H

#include "Lexer.h"
#include <vector>

/**
 * Tokens are stored and managed during parsing by the TokenStorage class.
 * The existance of only one instance in the entire program is ensured by using the singleton pattern.
 */
class TokenStorage
{
private:
    static TokenStorage instance;       // Instance of Singleton pattern
    std::vector<Token> tokens;          // A vector which store tokens
    int currPosition;                   // The current position in the tokens vector
    Lexer *lexer;                       // Pointer to the lexer

    // Private constructor which avoids instantiation
    TokenStorage() {}

    // Private destructor which avoids deletion of the instance
    ~TokenStorage() {}

    // Private copy constructor and assignment operator which prevent duplicating the instance
    TokenStorage(const TokenStorage &) = delete;
    TokenStorage &operator=(const TokenStorage &) = delete;

    /**
     * Tokens are retrieved from the lexer and stored in the tokens vector until the encounter of the token which represents end of file
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
     * Called when the TokenStorage instance is no longer needed.
     */
    static void clearInstance()
    {
        instance.lexer = 0;
        instance.tokens.clear();
    }
};

TokenStorage TokenStorage::instance; // Initialize the static instance

#endif // TOKENSSTORAGE_H
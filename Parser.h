#ifndef RPAL_CLION_PARSER_H
#define RPAL_CLION_PARSER_H
#include <vector>
#include "Token.h"
#include "Lexer.h"
#include "TokenStorage.h"
#include "Tree.h"
#include "TreeNode.h"


void parseExpression();
void parseExpressionWithWhere();

void parseTerm();
void parseTermWithAugmentation();
void parseTermWithCondition();

void parseBooleanExpression();
void parseBooleanTerm();
void parseBooleanFactor();
void parseBooleanPredicate();

void parseArithmeticExpression();
void parseArithmeticTerm();
void parseArithmeticFactor();
void parseArithmeticPower();

void parseAtomicExpression();
void parseAtomicPrimary();

void parseDeclaration();
void parseDeclarationWithWithinClause();
void parseRecursiveDeclaration();
void parseDeclarationBody();

void parseVariableBinding();
void parseVariableList();

// Constructing an AST from token sequences.
class Parser
{
public:
    static std::vector<TreeNode *> nodeStack;
    static void parse()
    {
        TokenStorage &tokenStorage = TokenStorage::getInstance();
        Token token = tokenStorage.top();

        // Check if the input token is the end of file token
        if (token.type == token_type::END_OF_FILE)
        {
            return; // No further parsing required, return from the function
        }
        else
        {
            parseExpression(); // Start parsing the expression

            // Check if the next token is the end of file token
            if (tokenStorage.top().type == token_type::END_OF_FILE)
            {
                // Set the root of the AST to the last node in the nodeStack
                Tree::getInstance().setASTRoot(Parser::nodeStack.back());
                return; // Parsing completed, return from the function
            }
            else
            {
                throw std::runtime_error("Syntax Error: end of file expected");
            }
        }
    }
};

std::vector<TreeNode *> Parser::nodeStack;

/**
 * Constructs a new TreeNode with the specified label, number of children, leaf status, and value.
 * Adds the constructed node to the nodeStack.
 * @param label The label of the node.
 * @param num The number of children the node will have.
 * @param isLeaf A boolean indicating whether the node is a leaf node or not.
 * @param value The value associated with the node (only applicable for leaf nodes).
 */
void build_tree(const std::string &label, const int &num, const bool isLeaf, const std::string &value = "")
{
    TreeNode *node;

    // Create a leaf node if isLeaf is true, otherwise create an internal node
    if (isLeaf)
    {
        node = new LeafNode(label, value);
    }
    else
    {
        node = new InternalNode(label);
    }

    // Add the children from the nodeStack to the newly created node
    for (int i = 0; i < num; i++)
    {
        node->addChild(Parser::nodeStack.back());
        Parser::nodeStack.pop_back();
    }

    // Reverse the order of the children
    node->reverseChildren();

    // Push the constructed node onto the nodeStack
    Parser::nodeStack.push_back(node);
}

/**
 * Parses the expression starting with E.
 * Handles the grammar rule E -> "let" D "in" E | "fn" Vb { Vb } "." E | Ew.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseExpression()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();

    // Check if the current token is "let"
    if (tokenStorage.top().value == "let")
    {
        tokenStorage.pop();
        parseDeclaration();

        // Check if the next token is "in"
        if (tokenStorage.top().value == "in")
        {
            tokenStorage.pop();
            parseExpression();
        }
        else
        {
            throw std::runtime_error("Syntax Error: 'in' expected");
        }

        // Build the "let" node with 2 children
        build_tree("let", 2, false);
    }
    // Check if the current token is "fn"
    else if (tokenStorage.top().value == "fn")
    {
        tokenStorage.pop();
        int n = 0;

        // Process identifiers until a non-identifier token is encountered
        while (tokenStorage.top().type == token_type::IDENTIFIER)
        {
            parseVariableBinding();
            n++;
        }

        if (n == 0)
        {
            throw std::runtime_error("Syntax Error: at least one identifier expected");
        }

        // Check if the next token is "."
        if (tokenStorage.top().value == ".")
        {
            tokenStorage.pop();
            parseExpression();
        }
        else
        {
            throw std::runtime_error("Syntax Error: '.' expected");
        }

        // Build the "lambda" node with n+1 children
        build_tree("lambda", n + 1, false);
    }
    else
    {
        parseExpressionWithWhere();
    }
}

/**
 * Parses the expression starting with Ew.
 * Handles the grammar rule Ew -> T [ "where" Dr ].
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseExpressionWithWhere()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    parseTerm();

    // Check if the next token is "where"
    if (tokenStorage.top().value == "where")
    {
        tokenStorage.pop();
        parseRecursiveDeclaration();
        build_tree("where", 2, false);
    }
}

/**
 * Parses the expression starting with T.
 * Handles the grammar rule T -> Ta { "," Ta }.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseTerm()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    parseTermWithAugmentation();
    int n = 0;

    // Process additional T expressions separated by commas
    while (tokenStorage.top().value == ",")
    {
        tokenStorage.pop();
        parseTermWithAugmentation();
        n++;
    }

    if (n > 0)
    {
        build_tree("tau", n + 1, false);
    }
}

/**
 * Parses the expression starting with Ta.
 * Handles the grammar rule Ta -> Tc { "aug" Tc }.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseTermWithAugmentation()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    parseTermWithCondition();

    // Process additional Tc expressions separated by "aug" keyword
    while (tokenStorage.top().value == "aug")
    {
        tokenStorage.pop();
        parseTermWithCondition();
        build_tree("aug", 2, false);
    }
}

/**
 * Parses the expression starting with Tc.
 * Handles the grammar rule Tc -> B [ "->" Tc [ "|" Tc ] ].
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseTermWithCondition()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    parseBooleanExpression();

    // Check if the next token is "->"
    if (tokenStorage.top().value == "->")
    {
        tokenStorage.pop();
        parseTermWithCondition();

        // Check if the next token is "|"
        if (tokenStorage.top().value == "|")
        {
            tokenStorage.pop();
            parseTermWithCondition();
            build_tree("->", 3, false);
        }
        else
        {
            throw std::runtime_error("Syntax Error: '|' expected");
        }
    }
}

/**
 * Parses the expression starting with B.
 * Handles the grammar rule B -> Bt { "or" Bt }.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseBooleanExpression()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    parseBooleanTerm();

    // Process additional Bt expressions separated by "or" keyword
    while (tokenStorage.top().value == "or")
    {
        tokenStorage.pop();
        parseBooleanTerm();
        build_tree("or", 2, false);
    }
}

/**
 * Parses the expression starting with Bt.
 * Handles the grammar rule Bt -> Bs { "&" Bs }.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseBooleanTerm()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    parseBooleanFactor();

    // Process additional Bs expressions separated by "&" keyword
    while (tokenStorage.top().value == "&")
    {
        tokenStorage.pop();
        parseBooleanFactor();
        build_tree("&", 2, false);
    }
}

/**
 * Parses the expression starting with Bs.
 * Handles the grammar rule Bs -> "not" Bp | Bp.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseBooleanFactor()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    if (tokenStorage.top().value == "not")
    {
        tokenStorage.pop();
        parseBooleanPredicate();
        build_tree("not", 1, false);
    }
    else
    {
        parseBooleanPredicate();
    }
}

/**
 * Parses the expression starting with Bp.
 * Handles the grammar rule Bp -> A { comparison_operator A }.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseBooleanPredicate()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    parseArithmeticExpression();

    // Check for comparison operators
    if (tokenStorage.top().value == "gr" || tokenStorage.top().value == ">")
    {
        tokenStorage.pop();
        parseArithmeticExpression();
        build_tree("gr", 2, false);
    }
    else if (tokenStorage.top().value == "ge" || tokenStorage.top().value == ">=")
    {
        tokenStorage.pop();
        parseArithmeticExpression();
        build_tree("ge", 2, false);
    }
    else if (tokenStorage.top().value == "ls" || tokenStorage.top().value == "<")
    {
        tokenStorage.pop();
        parseArithmeticExpression();
        build_tree("ls", 2, false);
    }
    else if (tokenStorage.top().value == "le" || tokenStorage.top().value == "<=")
    {
        tokenStorage.pop();
        parseArithmeticExpression();
        build_tree("le", 2, false);
    }
    else if (tokenStorage.top().value == "eq" || tokenStorage.top().value == "=")
    {
        tokenStorage.pop();
        parseArithmeticExpression();
        build_tree("eq", 2, false);
    }
    else if (tokenStorage.top().value == "ne" || tokenStorage.top().value == "!=")
    {
        tokenStorage.pop();
        parseArithmeticExpression();
        build_tree("ne", 2, false);
    }
}

/**
 * Parses the expression starting with A.
 * Handles the grammar rule A -> + At | - At | At { + At | - At }.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseArithmeticExpression()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();

    // Check for unary plus operator
    if (tokenStorage.top().value == "+")
    {
        tokenStorage.pop();
        parseArithmeticTerm();
    }
    // Check for unary minus operator
    else if (tokenStorage.top().value == "-")
    {
        tokenStorage.pop();
        parseArithmeticTerm();
        build_tree("neg", 1, false);
    }
    else
    {
        parseArithmeticTerm();
    }

    // Check for addition and subtraction operators
    while (tokenStorage.top().value == "+" || tokenStorage.top().value == "-")
    {
        if (tokenStorage.top().value == "+")
        {
            tokenStorage.pop();
            parseArithmeticTerm();
            build_tree("+", 2, false);
        }
        else if (tokenStorage.top().value == "-")
        {
            tokenStorage.pop();
            parseArithmeticTerm();
            build_tree("-", 2, false);
        }
    }
}

/**
 * Parses the expression starting with At.
 * Handles the grammar rule At -> Af { * Af | / Af }.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseArithmeticTerm()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    parseArithmeticFactor();

    // Check for multiplication and division operators
    while (tokenStorage.top().value == "*" || tokenStorage.top().value == "/")
    {
        if (tokenStorage.top().value == "*")
        {
            tokenStorage.pop();
            parseArithmeticFactor();
            build_tree("*", 2, false);
        }
        else if (tokenStorage.top().value == "/")
        {
            tokenStorage.pop();
            parseArithmeticFactor();
            build_tree("/", 2, false);
        }
    }
}

/**
 * Parses the expression starting with Af.
 * Handles the grammar rule Af -> Ap { ** Ap }.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseArithmeticFactor()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    parseArithmeticPower();

    // Check for exponentiation operator
    while (tokenStorage.top().value == "**")
    {
        tokenStorage.pop();
        parseArithmeticPower();
        build_tree("**", 2, false);
    }
}

/**
 * Parses the expression starting with Ap.
 * Handles the grammar rule Ap -> R { @ identifier R }.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseArithmeticPower()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    parseAtomicExpression();

    // Check for function application operator
    while (tokenStorage.top().value == "@")
    {
        tokenStorage.pop();

        // Check for identifier token
        if (tokenStorage.top().type == token_type::IDENTIFIER)
        {
            Token token = tokenStorage.pop();
            build_tree("identifier", 0, true, token.value);
        }
        else
        {
            throw std::runtime_error("Syntax Error: Identifier expected");
        }

        parseAtomicExpression();
        build_tree("@", 3, false);
    }
}

/**
 * Parses the expression starting with R.
 * Handles the grammar rule R -> Rn { Rn }.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseAtomicExpression()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    parseAtomicPrimary();

    Token top = tokenStorage.top();
    while (top.type == token_type::IDENTIFIER || top.type == token_type::INTEGER || top.type == token_type::STRING || top.value == "true" || top.value == "false" || top.value == "nil" || top.value == "(" || top.value == "dummy")
    {
        parseAtomicPrimary();
        top = tokenStorage.top();
        build_tree("gamma", 2, false);
    }
}

/**
 * Parses the expression starting with Rn.
 * Handles the grammar rule Rn -> identifier | integer | string | true | false | nil | ( E ) | dummy.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseAtomicPrimary()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    Token top = tokenStorage.top();

    if (top.type == token_type::IDENTIFIER)
    {
        // Parse Identifier
        Token token = tokenStorage.pop();
        build_tree("identifier", 0, true, token.value);
    }
    else if (top.type == token_type::INTEGER)
    {
        // Parse Integer
        Token token = tokenStorage.pop();
        build_tree("integer", 0, true, token.value);
    }
    else if (top.type == token_type::STRING)
    {
        // Parse String
        Token token = tokenStorage.pop();
        build_tree("string", 0, true, token.value);
    }
    else if (top.value == "true")
    {
        // Parse true
        tokenStorage.pop();
        build_tree("true", 0, true);
    }
    else if (top.value == "false")
    {
        // Parse false
        tokenStorage.pop();
        build_tree("false", 0, true);
    }
    else if (top.value == "nil")
    {
        // Parse nil
        tokenStorage.pop();
        build_tree("nil", 0, true);
    }
    else if (top.value == "(")
    {
        tokenStorage.pop();
        parseExpression();
        if (tokenStorage.top().value == ")")
        {
            tokenStorage.pop();
        }
        else
        {
            throw std::runtime_error("Syntax Error: ')' expected");
        }
    }
    else if (top.value == "dummy")
    {
        // Parse dummy
        tokenStorage.pop();
        build_tree("dummy", 0, true);
    }
    else
    {
        throw std::runtime_error("Syntax Error: Identifier, Integer, String, 'true', 'false', 'nil', '(', 'dummy' expected\ngot: " + top.value);
    }
}

/**
 * Parses the expression starting with D.
 * Handles the grammar rule D -> Da [ within D ].
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseDeclaration()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    parseDeclarationWithWithinClause();

    while (tokenStorage.top().value == "within")
    {
        tokenStorage.pop();
        parseDeclaration();
        build_tree("within", 2, false);
    }
}

/**
 * Parses the expression starting with Da.
 * Handles the grammar rule Da -> Dr { and Dr }.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseDeclarationWithWithinClause()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    parseRecursiveDeclaration();
    int n = 0;

    while (tokenStorage.top().value == "and")
    {
        tokenStorage.pop();
        parseRecursiveDeclaration();
        n++;
    }
    if (n > 0) {
        build_tree("and", n + 1, false);
    }
}

/**
 * Parses the expression starting with Dr.
 * Handles the grammar rule Dr -> rec Db | Db.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseRecursiveDeclaration()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();

    if (tokenStorage.top().value == "rec")
    {
        tokenStorage.pop();
        parseDeclarationBody();
        build_tree("rec", 1, false);
    }
    else
    {
        parseDeclarationBody();
    }
}

/**
 * Parses the expression starting with Db.
 * Handles the grammar rule Db -> ( D ) | identifier Vl = E | Vb { , Vb } = E | epsilon.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseDeclarationBody()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();

    if (tokenStorage.top().value == "(")
    {
        tokenStorage.pop();
        parseDeclaration();

        if (tokenStorage.top().value == ")")
        {
            tokenStorage.pop();
        }
        else
        {
            throw std::runtime_error("Syntax Error: ')' expected");
        }
    }
    else if (tokenStorage.top().type == token_type::IDENTIFIER)
    {
        // Parse Identifier
        Token token = tokenStorage.pop();
        build_tree("identifier", 0, true, token.value);

        if (tokenStorage.top().value == ",")
        {
            tokenStorage.pop();
            parseVariableList();

            if (tokenStorage.top().value == "=")
            {
                tokenStorage.pop();
                parseExpression();
                build_tree("=", 2, false);
            }
            else
            {
                throw std::runtime_error("Syntax Error: '=' expected");
            }
        }
        else
        {
            int n = 0;

            while (tokenStorage.top().value != "=" && tokenStorage.top().type == token_type::IDENTIFIER)
            {
                parseVariableBinding();
                n++;
            }

            if (tokenStorage.top().value == "(")
            {
                //                tokenStorage.pop();
                //                while (tokenStorage.top().value != ")")
                //                {
                //                    parseVariableBinding();
                //                    if (tokenStorage.top().value == ",")
                //                    {
                //                        tokenStorage.pop();
                //                    }
                //                    else
                //                    {
                //                        n++;
                //                        break;
                //                    }
                //                    n++;
                //                }
                //
                //                if (tokenStorage.top().value == ")") {
                //                    tokenStorage.pop();
                //                }
                //                else {
                //                    throw std::runtime_error("Syntax Error: ')' expected");
                //                }
                parseVariableBinding();
                n++;
            }

            if (n == 0 && tokenStorage.top().value == "=")
            {
                tokenStorage.pop();
                parseExpression();
                build_tree("=", 2, false);
            }
            else if (n != 0 && tokenStorage.top().value == "=")
            {
                tokenStorage.pop();
                parseExpression();
                build_tree("fcn_form", n + 2, false);
            }
            else
            {
                throw std::runtime_error("Syntax Error: '=' expected");
            }
        }
    }
    else
    {
        throw std::runtime_error("Syntax Error: '(' or Identifier expected");
    }
}

/**
 * Parses the expression starting with Vb.
 * Handles the grammar rule Vb -> identifier | ( ) | ( identifier Vl ).
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseVariableBinding()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();

    if (tokenStorage.top().type == token_type::IDENTIFIER)
    {
        // Parse Identifier
        Token token = tokenStorage.pop();
        build_tree("identifier", 0, true, token.value);
    }
    else if (tokenStorage.top().value == "(")
    {
        tokenStorage.pop();

        if (tokenStorage.top().value == ")")
        {
            tokenStorage.pop();
            build_tree("()", 0, true);
        }
        else if (tokenStorage.top().type == token_type::IDENTIFIER)
        {
            // Parse Identifier
            Token token = tokenStorage.pop();
            build_tree("identifier", 0, true, token.value);

            if (tokenStorage.top().value == ",")
            {
                tokenStorage.pop();
                parseVariableList();
            }
            //            else
            //            {
            //                throw std::runtime_error("Syntax Error: ',' expected");
            //            }

            if (tokenStorage.top().value == ")")
            {
                tokenStorage.pop();
            }
            else
            {
                throw std::runtime_error("Syntax Error: ')' expected");
            }
        }
        else
        {
            throw std::runtime_error("Syntax Error: Identifier or ')' expected");
        }
    }
    else
    {
        throw std::runtime_error("Syntax Error: Identifier or '(' expected");
    }
}

/**
 * Parses the expression starting with Vl.
 * Handles the grammar rule Vl -> identifier { , identifier }.
 * Constructs the Abstract Syntax Tree (AST) nodes and builds the tree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void parseVariableList()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();

    if (tokenStorage.top().type == token_type::IDENTIFIER)
    {
        // Parse Identifier
        Token token = tokenStorage.pop();
        build_tree("identifier", 0, true, token.value);

        int n = 2;
        while (tokenStorage.top().value == ",")
        {
            tokenStorage.pop();
            token = tokenStorage.pop();
            build_tree("identifier", 0, true, token.value);
            n++;
        }

        build_tree(",", n, false);
    }
    else
    {
        throw std::runtime_error("Syntax Error: Identifier expected");
    }
}

#endif
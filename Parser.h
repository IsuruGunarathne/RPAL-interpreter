#ifndef RPAL_CLION_PARSER_H
#define RPAL_CLION_PARSER_H

#include "Token.h"
#include "Lexer.h"
#include "TokenStorage.h"
#include "Tree.h"
#include "TreeNode.h"

#include <vector>

// function prototypes
void E();
void Ew();
void T();
void Ta();
void Tc();
void B();
void Bt();
void Bs();
void Bp();
void A();
void At();
void Af();
void Ap();
void R();
void Rn();
void D();
void Da();
void Dr();
void Db();
void Vb();
void Vl();

/**
 * The Parser class is responsible for parsing a sequence of tokens and constructing the Abstract Syntax CustomTree (AST).
 */
class Parser
{
public:
    static std::vector<CustomTreeNode *> nodeStack;

    /**
     * Parses the input tokens and constructs the Abstract Syntax CustomTree (AST).
     */
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
            E(); // Start parsing the expression

            // Check if the next token is the end of file token
            if (tokenStorage.top().type == token_type::END_OF_FILE)
            {
                // Set the root of the AST to the last node in the nodeStack
                CustomTree::getInstance().setASTRoot(Parser::nodeStack.back());
                return; // Parsing completed, return from the function
            }
            else
            {
                throw std::runtime_error("Syntax Error: end of file expected");
            }
        }
    }
};

std::vector<CustomTreeNode *> Parser::nodeStack;

/**
 * Constructs a new CustomTreeNode with the specified nodeLabel, number of children, leaf status, and nodeValue.
 * Adds the constructed node to the nodeStack.
 * @param nodeLabel The nodeLabel of the node.
 * @param num The number of children the node will have.
 * @param isLeaf A boolean indicating whether the node is a leaf node or not.
 * @param nodeValue The nodeValue associated with the node (only applicable for leaf nodes).
 */
void build_tree(const std::string &nodeLabel, const int &num, const bool isLeaf, const std::string &nodeValue = "")
{
    CustomTreeNode *node;

    // Create a leaf node if isLeaf is true, otherwise create an internal node
    if (isLeaf)
    {
        node = new LeafNode(nodeLabel, nodeValue);
    }
    else
    {
        node = new InternalNode(nodeLabel);
    }

    // Add the children from the nodeStack to the newly created node
    for (int i = 0; i < num; i++)
    {
        node->appendChild(Parser::nodeStack.back());
        Parser::nodeStack.pop_back();
    }

    // Reverse the order of the children
    node->reverseChildrenOrder();

    // Push the constructed node onto the nodeStack
    Parser::nodeStack.push_back(node);
}

/**
 * Parses the expression starting with E.
 * Handles the grammar rule E -> "let" D "in" E | "fn" Vb { Vb } "." E | Ew.
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void E()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();

    // Check if the current token is "let"
    if (tokenStorage.top().nodeValue == "let")
    {
        tokenStorage.pop();
        D();

        // Check if the next token is "in"
        if (tokenStorage.top().nodeValue == "in")
        {
            tokenStorage.pop();
            E();
        }
        else
        {
            throw std::runtime_error("Syntax Error: 'in' expected");
        }

        // Build the "let" node with 2 children
        build_tree("let", 2, false);
    }
    // Check if the current token is "fn"
    else if (tokenStorage.top().nodeValue == "fn")
    {
        tokenStorage.pop();
        int n = 0;

        // Process identifiers until a non-identifier token is encountered
        while (tokenStorage.top().type == token_type::IDENTIFIER)
        {
            Vb();
            n++;
        }

        if (n == 0)
        {
            throw std::runtime_error("Syntax Error: at least one identifier expected");
        }

        // Check if the next token is "."
        if (tokenStorage.top().nodeValue == ".")
        {
            tokenStorage.pop();
            E();
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
        Ew();
    }
}

/**
 * Parses the expression starting with Ew.
 * Handles the grammar rule Ew -> T [ "where" Dr ].
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void Ew()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    T();

    // Check if the next token is "where"
    if (tokenStorage.top().nodeValue == "where")
    {
        tokenStorage.pop();
        Dr();
        build_tree("where", 2, false);
    }
}

/**
 * Parses the expression starting with T.
 * Handles the grammar rule T -> Ta { "," Ta }.
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void T()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    Ta();
    int n = 0;

    // Process additional T expressions separated by commas
    while (tokenStorage.top().nodeValue == ",")
    {
        tokenStorage.pop();
        Ta();
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
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void Ta()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    Tc();

    // Process additional Tc expressions separated by "aug" keyword
    while (tokenStorage.top().nodeValue == "aug")
    {
        tokenStorage.pop();
        Tc();
        build_tree("aug", 2, false);
    }
}

/**
 * Parses the expression starting with Tc.
 * Handles the grammar rule Tc -> B [ "->" Tc [ "|" Tc ] ].
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void Tc()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    B();

    // Check if the next token is "->"
    if (tokenStorage.top().nodeValue == "->")
    {
        tokenStorage.pop();
        Tc();

        // Check if the next token is "|"
        if (tokenStorage.top().nodeValue == "|")
        {
            tokenStorage.pop();
            Tc();
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
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void B()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    Bt();

    // Process additional Bt expressions separated by "or" keyword
    while (tokenStorage.top().nodeValue == "or")
    {
        tokenStorage.pop();
        Bt();
        build_tree("or", 2, false);
    }
}

/**
 * Parses the expression starting with Bt.
 * Handles the grammar rule Bt -> Bs { "&" Bs }.
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void Bt()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    Bs();

    // Process additional Bs expressions separated by "&" keyword
    while (tokenStorage.top().nodeValue == "&")
    {
        tokenStorage.pop();
        Bs();
        build_tree("&", 2, false);
    }
}

/**
 * Parses the expression starting with Bs.
 * Handles the grammar rule Bs -> "not" Bp | Bp.
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void Bs()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    if (tokenStorage.top().nodeValue == "not")
    {
        tokenStorage.pop();
        Bp();
        build_tree("not", 1, false);
    }
    else
    {
        Bp();
    }
}

/**
 * Parses the expression starting with Bp.
 * Handles the grammar rule Bp -> A { comparison_operator A }.
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void Bp()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    A();

    // Check for comparison operators
    if (tokenStorage.top().nodeValue == "gr" || tokenStorage.top().nodeValue == ">")
    {
        tokenStorage.pop();
        A();
        build_tree("gr", 2, false);
    }
    else if (tokenStorage.top().nodeValue == "ge" || tokenStorage.top().nodeValue == ">=")
    {
        tokenStorage.pop();
        A();
        build_tree("ge", 2, false);
    }
    else if (tokenStorage.top().nodeValue == "ls" || tokenStorage.top().nodeValue == "<")
    {
        tokenStorage.pop();
        A();
        build_tree("ls", 2, false);
    }
    else if (tokenStorage.top().nodeValue == "le" || tokenStorage.top().nodeValue == "<=")
    {
        tokenStorage.pop();
        A();
        build_tree("le", 2, false);
    }
    else if (tokenStorage.top().nodeValue == "eq" || tokenStorage.top().nodeValue == "=")
    {
        tokenStorage.pop();
        A();
        build_tree("eq", 2, false);
    }
    else if (tokenStorage.top().nodeValue == "ne" || tokenStorage.top().nodeValue == "!=")
    {
        tokenStorage.pop();
        A();
        build_tree("ne", 2, false);
    }
}

/**
 * Parses the expression starting with A.
 * Handles the grammar rule A -> + At | - At | At { + At | - At }.
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void A()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();

    // Check for unary plus operator
    if (tokenStorage.top().nodeValue == "+")
    {
        tokenStorage.pop();
        At();
    }
    // Check for unary minus operator
    else if (tokenStorage.top().nodeValue == "-")
    {
        tokenStorage.pop();
        At();
        build_tree("neg", 1, false);
    }
    else
    {
        At();
    }

    // Check for addition and subtraction operators
    while (tokenStorage.top().nodeValue == "+" || tokenStorage.top().nodeValue == "-")
    {
        if (tokenStorage.top().nodeValue == "+")
        {
            tokenStorage.pop();
            At();
            build_tree("+", 2, false);
        }
        else if (tokenStorage.top().nodeValue == "-")
        {
            tokenStorage.pop();
            At();
            build_tree("-", 2, false);
        }
    }
}

/**
 * Parses the expression starting with At.
 * Handles the grammar rule At -> Af { * Af | / Af }.
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void At()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    Af();

    // Check for multiplication and division operators
    while (tokenStorage.top().nodeValue == "*" || tokenStorage.top().nodeValue == "/")
    {
        if (tokenStorage.top().nodeValue == "*")
        {
            tokenStorage.pop();
            Af();
            build_tree("*", 2, false);
        }
        else if (tokenStorage.top().nodeValue == "/")
        {
            tokenStorage.pop();
            Af();
            build_tree("/", 2, false);
        }
    }
}

/**
 * Parses the expression starting with Af.
 * Handles the grammar rule Af -> Ap { ** Ap }.
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void Af()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    Ap();

    // Check for exponentiation operator
    while (tokenStorage.top().nodeValue == "**")
    {
        tokenStorage.pop();
        Ap();
        build_tree("**", 2, false);
    }
}

/**
 * Parses the expression starting with Ap.
 * Handles the grammar rule Ap -> R { @ identifier R }.
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void Ap()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    R();

    // Check for function application operator
    while (tokenStorage.top().nodeValue == "@")
    {
        tokenStorage.pop();

        // Check for identifier token
        if (tokenStorage.top().type == token_type::IDENTIFIER)
        {
            Token token = tokenStorage.pop();
            build_tree("identifier", 0, true, token.nodeValue);
        }
        else
        {
            throw std::runtime_error("Syntax Error: Identifier expected");
        }

        R();
        build_tree("@", 3, false);
    }
}

/**
 * Parses the expression starting with R.
 * Handles the grammar rule R -> Rn { Rn }.
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void R()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    Rn();

    Token top = tokenStorage.top();
    while (top.type == token_type::IDENTIFIER || top.type == token_type::INTEGER || top.type == token_type::STRING || top.nodeValue == "true" || top.nodeValue == "false" || top.nodeValue == "nil" || top.nodeValue == "(" || top.nodeValue == "dummy")
    {
        Rn();
        top = tokenStorage.top();
        build_tree("gamma", 2, false);
    }
}

/**
 * Parses the expression starting with Rn.
 * Handles the grammar rule Rn -> identifier | integer | string | true | false | nil | ( E ) | dummy.
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void Rn()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    Token top = tokenStorage.top();

    if (top.type == token_type::IDENTIFIER)
    {
        // Parse Identifier
        Token token = tokenStorage.pop();
        build_tree("identifier", 0, true, token.nodeValue);
    }
    else if (top.type == token_type::INTEGER)
    {
        // Parse Integer
        Token token = tokenStorage.pop();
        build_tree("integer", 0, true, token.nodeValue);
    }
    else if (top.type == token_type::STRING)
    {
        // Parse String
        Token token = tokenStorage.pop();
        build_tree("string", 0, true, token.nodeValue);
    }
    else if (top.nodeValue == "true")
    {
        // Parse true
        tokenStorage.pop();
        build_tree("true", 0, true);
    }
    else if (top.nodeValue == "false")
    {
        // Parse false
        tokenStorage.pop();
        build_tree("false", 0, true);
    }
    else if (top.nodeValue == "nil")
    {
        // Parse nil
        tokenStorage.pop();
        build_tree("nil", 0, true);
    }
    else if (top.nodeValue == "(")
    {
        tokenStorage.pop();
        E();
        if (tokenStorage.top().nodeValue == ")")
        {
            tokenStorage.pop();
        }
        else
        {
            throw std::runtime_error("Syntax Error: ')' expected");
        }
    }
    else if (top.nodeValue == "dummy")
    {
        // Parse dummy
        tokenStorage.pop();
        build_tree("dummy", 0, true);
    }
    else
    {
        throw std::runtime_error("Syntax Error: Identifier, Integer, String, 'true', 'false', 'nil', '(', 'dummy' expected\ngot: " + top.nodeValue);
    }
}

/**
 * Parses the expression starting with D.
 * Handles the grammar rule D -> Da [ within D ].
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void D()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    Da();

    while (tokenStorage.top().nodeValue == "within")
    {
        tokenStorage.pop();
        D();
        build_tree("within", 2, false);
    }
}

/**
 * Parses the expression starting with Da.
 * Handles the grammar rule Da -> Dr { and Dr }.
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void Da()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();
    Dr();
    int n = 0;

    while (tokenStorage.top().nodeValue == "and")
    {
        tokenStorage.pop();
        Dr();
        n++;
    }
    if (n > 0) {
        build_tree("and", n + 1, false);
    }
}

/**
 * Parses the expression starting with Dr.
 * Handles the grammar rule Dr -> rec Db | Db.
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void Dr()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();

    if (tokenStorage.top().nodeValue == "rec")
    {
        tokenStorage.pop();
        Db();
        build_tree("rec", 1, false);
    }
    else
    {
        Db();
    }
}

/**
 * Parses the expression starting with Db.
 * Handles the grammar rule Db -> ( D ) | identifier Vl = E | Vb { , Vb } = E | epsilon.
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void Db()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();

    if (tokenStorage.top().nodeValue == "(")
    {
        tokenStorage.pop();
        D();

        if (tokenStorage.top().nodeValue == ")")
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
        build_tree("identifier", 0, true, token.nodeValue);

        if (tokenStorage.top().nodeValue == ",")
        {
            tokenStorage.pop();
            Vl();

            if (tokenStorage.top().nodeValue == "=")
            {
                tokenStorage.pop();
                E();
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

            while (tokenStorage.top().nodeValue != "=" && tokenStorage.top().type == token_type::IDENTIFIER)
            {
                Vb();
                n++;
            }

            if (tokenStorage.top().nodeValue == "(")
            {
                //                tokenStorage.pop();
                //                while (tokenStorage.top().nodeValue != ")")
                //                {
                //                    Vb();
                //                    if (tokenStorage.top().nodeValue == ",")
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
                //                if (tokenStorage.top().nodeValue == ")") {
                //                    tokenStorage.pop();
                //                }
                //                else {
                //                    throw std::runtime_error("Syntax Error: ')' expected");
                //                }
                Vb();
                n++;
            }

            if (n == 0 && tokenStorage.top().nodeValue == "=")
            {
                tokenStorage.pop();
                E();
                build_tree("=", 2, false);
            }
            else if (n != 0 && tokenStorage.top().nodeValue == "=")
            {
                tokenStorage.pop();
                E();
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
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void Vb()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();

    if (tokenStorage.top().type == token_type::IDENTIFIER)
    {
        // Parse Identifier
        Token token = tokenStorage.pop();
        build_tree("identifier", 0, true, token.nodeValue);
    }
    else if (tokenStorage.top().nodeValue == "(")
    {
        tokenStorage.pop();

        if (tokenStorage.top().nodeValue == ")")
        {
            tokenStorage.pop();
            build_tree("()", 0, true);
        }
        else if (tokenStorage.top().type == token_type::IDENTIFIER)
        {
            // Parse Identifier
            Token token = tokenStorage.pop();
            build_tree("identifier", 0, true, token.nodeValue);

            if (tokenStorage.top().nodeValue == ",")
            {
                tokenStorage.pop();
                Vl();
            }
            //            else
            //            {
            //                throw std::runtime_error("Syntax Error: ',' expected");
            //            }

            if (tokenStorage.top().nodeValue == ")")
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
 * Constructs the Abstract Syntax CustomTree (AST) nodes and builds the customTree accordingly.
 *
 * @throws std::runtime_error if a syntax error occurs.
 */
void Vl()
{
    TokenStorage &tokenStorage = TokenStorage::getInstance();

    if (tokenStorage.top().type == token_type::IDENTIFIER)
    {
        // Parse Identifier
        Token token = tokenStorage.pop();
        build_tree("identifier", 0, true, token.nodeValue);

        int n = 2;
        while (tokenStorage.top().nodeValue == ",")
        {
            tokenStorage.pop();
            token = tokenStorage.pop();
            build_tree("identifier", 0, true, token.nodeValue);
            n++;
        }

        build_tree(",", n, false);
    }
    else
    {
        throw std::runtime_error("Syntax Error: Identifier expected");
    }
}

#endif // RPAL_CLION_PARSER_H

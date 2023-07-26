#ifndef CUSTOM_TREE_H
#define CUSTOM_TREE_H

#include "TreeNode.h"

void generateSyntaxTree(CustomTreeNode *currentNode, CustomTreeNode *parentNode);

/**
 * @brief Represents the Custom Tree for a program.
 *
 * The CustomTree class stores the root nodes of the AST and ST and provides
 * access to them. It follows the Singleton design pattern to ensure
 * that only one instance of the CustomTree exists.
 */
class CustomTree
{
private:
    static CustomTree *customTree; // The singleton instance of the CustomTree          // The singleton instance of the CustomTree
    CustomTreeNode *astRoot = nullptr; // The root node of the Abstract Syntax CustomTree (AST)
    CustomTreeNode *stRoot = nullptr;  // The root node of the Standardized CustomTree (ST)

    CustomTree() {} // Private constructor

    ~CustomTree() {} // Private destructor

    CustomTree(const CustomTree &) = delete; // Disable copy constructor

    CustomTree &operator=(const CustomTree &) = delete; // Disable assignment operator

public:
    /**
     * @brief Sets the root node of the Abstract Syntax Tree (AST).
     * @param root The root node to set.
     */
    static CustomTree &getInstance()
    {
        return *customTree;
    }

    /**
     * @brief Sets the root node of the Abstract Syntax CustomTree (AST).
     * @param root The root node to set.
     */
    void setASTRoot(CustomTreeNode *root)
    {
        astRoot = root;
    }

    /**
     * @brief Retrieves the root node of the Abstract Syntax CustomTree (AST).
     * @return The root node of the AST.
     */
    CustomTreeNode *getASTRoot()
    {
        return astRoot;
    }
        /**
     * @brief Retrieves the root node of the Standardized CustomTree (ST).
     * @return The root node of the ST.
     */
    CustomTreeNode *getSTRoot()
    {
        return stRoot;
    }
    /**
     * @brief Sets the root node of the Standardized CustomTree (ST).
     * @param root The root node to set.
     */
    void setSTRoot(CustomTreeNode *root)
    {
        stRoot = root;
    }



    /**
     * @brief Releases the memory occupied by the Abstract Syntax CustomTree (AST).
     *
     * This function sets the AST root node to nullptr to release the memory
     * occupied by the AST. It should be called when the AST is no longer needed
     * to avoid memory leaks.
     */
    static void releaseASTMemory()
    {
        if (customTree->astRoot != nullptr)
        {
            if (customTree->stRoot == nullptr)
            {
                customTree->stRoot = customTree->astRoot;
            }

            customTree->astRoot = nullptr;
        }
    }

    /**
     * @brief Releases the memory occupied by the Standardized CustomTree (ST).
     *
     * This function calls the static deleteNodeMemory() function
     * of the CustomTreeNode class to release the memory of all ST nodes.
     * It should be called when the ST is no longer needed to avoid memory leaks.
     */
    static void releaseSTMemory()
    {
        CustomTreeNode::deleteNodeMemory(customTree->stRoot);
    }

    /**
     * @brief Generates the Standardized CustomTree (ST) from the Abstract Syntax CustomTree (AST).
     *
     * This function calls the generateSyntaxTree() function to generate the ST from the AST.
     * It should be called when the AST is no longer needed to avoid memory leaks.
     */
    static void generate()
    {
        releaseASTMemory();
        generateSyntaxTree(customTree->stRoot, nullptr);
    }
};

CustomTree *CustomTree::customTree = new CustomTree(); // Initialize the singleton instance of the CustomTree

/**
 * Generates the Syntax CustomTree (ST) by modifying the given customTree structure.
 */
void generateSyntaxTree(CustomTreeNode *currentNode, CustomTreeNode *parentNode = nullptr)
{
    if (currentNode == nullptr)
    {
        return; 
    }

    if (currentNode->getChildCount() != 0)
    {
        std::vector<CustomTreeNode *> children = currentNode->getChildren(); // Get the children of the current node
        for (CustomTreeNode *child : children)
        {
            generateSyntaxTree(child, currentNode); // Recursively generate the syntax customTree for each child
            currentNode->removeChild(0);    // Remove the processed child from the current node
        }
    }

    CustomTreeNode *root_node; // Assign the current node as the root node of the syntax customTree

    // Define a static vector of binary operators
    static std::vector<std::string> binaryOperators = {
        "+", "-", "*", "/", "**", "gr", "ge", "ls", "le", "aug", "or", "&", "eq", "ne"};
    // Identify and process specific node types
    if (currentNode->getLabel() == "let")
    {   // Process "let" nodes
        if (currentNode->getChildCount() == 2)
        {
            std::vector<CustomTreeNode *> children = currentNode->getChildren();

            CustomTreeNode *eq_node;
            CustomTreeNode *p_node;

            // Check if the first child is the "=" node
            if (children[0]->getLabel() == "=")
            {
                eq_node = children[0];
                p_node = children[1];
            }
            // Check if the second child is the "=" node
            else if (children[1]->getLabel() == "=")
            {
                eq_node = children[1];
                p_node = children[0];
            }
            // Throw an error if neither child is the "=" node
            else
            {
                throw std::runtime_error("Error: let node does not have an = node as a child");
            }

            if (eq_node->getChildCount() == 2)
            {
                CustomTreeNode *lambda_node = new InternalNode("lambda");
                CustomTreeNode *gamma_node = new InternalNode("gamma");

                CustomTreeNode *var_node = eq_node->getChildren()[0];
                CustomTreeNode *expr_node = eq_node->getChildren()[1];

                lambda_node->appendChild(var_node);
                lambda_node->appendChild(p_node);

                gamma_node->appendChild(lambda_node);
                gamma_node->appendChild(expr_node);

                root_node = gamma_node; // Assign the gamma_node as the new root node
                                        //            parentNode->appendChild(gamma_node);
            }
            else
            {
                throw std::runtime_error("Error: = node must only have 2 children.");
            }
        }
        else
        {
            throw std::runtime_error("Error: let node must have 2 children.");
        }
    }
    else if (currentNode->getLabel() == "where")
    {
        if (currentNode->getChildCount() == 2)
        {
            std::vector<CustomTreeNode *> children = currentNode->getChildren();

            CustomTreeNode *eq_node;
            CustomTreeNode *p_node;

            // Check if the first child is the "=" node
            if (children[0]->getLabel() == "=")
            {
                eq_node = children[0];
                p_node = children[1];
            }
            // Check if the second child is the "=" node
            else if (children[1]->getLabel() == "=")
            {
                eq_node = children[1];
                p_node = children[0];
            }
            // Throw an error if neither child is the "=" node
            else
            {
                throw std::runtime_error("Error: where node does not have an = node as a child");
            }

            if (eq_node->getChildCount() == 2)
            {
                CustomTreeNode *lambda_node = new InternalNode("lambda");
                CustomTreeNode *gamma_node = new InternalNode("gamma");

                CustomTreeNode *var_node = eq_node->getChildren()[0];
                CustomTreeNode *expr_node = eq_node->getChildren()[1];

                lambda_node->appendChild(var_node);
                lambda_node->appendChild(p_node);

                gamma_node->appendChild(lambda_node);
                gamma_node->appendChild(expr_node);

                root_node = gamma_node; // Assign the gamma_node as the new root node
                                        //            parentNode->appendChild(gamma_node);
            }
            else
            {
                throw std::runtime_error("Error: = node must only have 2 children.");
            }
        }
        else
        {
            throw std::runtime_error("Error: where node must have 2 children.");
        }
    }
    else if (currentNode->getLabel() == "fcn_form")
    {
        if (currentNode->getChildCount() > 2)
        {
            std::vector<CustomTreeNode *> children = currentNode->getChildren();

            CustomTreeNode *fcn_name_node = children.front();
            // Remove fcn_name_node from children
            children.erase(children.begin());

            CustomTreeNode *expr_node = children.back();
            // Remove expr_node from children
            children.pop_back();

            CustomTreeNode *eq_node = new InternalNode("=");

            eq_node->appendChild(fcn_name_node);

            CustomTreeNode *prev_node = eq_node;
            for (CustomTreeNode *child : children)
            {
                CustomTreeNode *lambda_node = new InternalNode("lambda");
                lambda_node->appendChild(child);
                prev_node->appendChild(lambda_node);
                prev_node = lambda_node;
            }

            prev_node->appendChild(expr_node);

            root_node = eq_node; // Assign the eq_node as the new root node
                                 //        parentNode->appendChild(eq_node);
        }
        else
        {
            throw std::runtime_error("Error: fcn_form node must have more than 2 children.");
        }
    }
    else if (currentNode->getLabel() == "tau")
    {
       
        root_node = currentNode;
     
    }
    else if (currentNode->getLabel() == "lambda" && currentNode->getChildren()[0]->getLabel() != "," &&
             currentNode->getChildren()[1]->getLabel() != ",")
    {
        if (currentNode->getChildCount() >= 2)
        {
            std::vector<CustomTreeNode *> children = currentNode->getChildren();

            CustomTreeNode *expr_node = children.back();
            // Remove expr_node from children
            children.pop_back();

            CustomTreeNode *head_lambda_node = new InternalNode("lambda");

            CustomTreeNode *prev_node = head_lambda_node;
            for (CustomTreeNode *child : children)
            {
                CustomTreeNode *lambda_node = new InternalNode("lambda");
                lambda_node->appendChild(child);
                prev_node->appendChild(lambda_node);
                prev_node = lambda_node;
            }

            prev_node->appendChild(expr_node);

            root_node = head_lambda_node->getChildren()[0];
            head_lambda_node->getChildren().erase(head_lambda_node->getChildren().begin());
            delete head_lambda_node;
            //        parentNode->appendChild(head_lambda_node);
        }
        else
        {
            throw std::runtime_error("Error: lambda node must have at least 2 children.");
        }
    }
    else if (currentNode->getLabel() == "within")
    {
        if (currentNode->getChildCount() == 2)
        {
            std::vector<CustomTreeNode *> children = currentNode->getChildren();

            // Check if each child is the "=" node and has exactly 2 children
            for (CustomTreeNode *child : children)
            {
                if (child->getLabel() != "=")
                {
                    throw std::runtime_error("Error: within node must have an = node as a child");
                }
                else if (child->getChildCount() != 2)
                {
                    throw std::runtime_error("Error: = node must have 2 children.");
                }
            }

            // Retrieve the two "=" nodes
            CustomTreeNode *first_eq_node = children[0];
            CustomTreeNode *second_eq_node = children[1];

            // Create new nodes for constructing the modified syntax customTree
            CustomTreeNode *new_eq_node = new InternalNode("=");
            CustomTreeNode *new_gamma_node = new InternalNode("gamma");
            CustomTreeNode *new_lambda_node = new InternalNode("lambda");

            // Modify the new_eq_node and new_gamma_node
            new_eq_node->appendChild(second_eq_node->getChildren()[0]);
            new_eq_node->appendChild(new_gamma_node);

            // Modify the new_gamma_node
            new_gamma_node->appendChild(new_lambda_node);
            new_gamma_node->appendChild(first_eq_node->getChildren()[1]);

            // Modify the new_lambda_node
            new_lambda_node->appendChild(first_eq_node->getChildren()[0]);
            new_lambda_node->appendChild(second_eq_node->getChildren()[1]);

            root_node = new_eq_node; // Assign the new_eq_node as the new root node
                                     //        parentNode->appendChild(new_eq_node);
        }
        else
        {
            throw std::runtime_error("Error: within node must have 2 children.");
        }
    }
    else if (currentNode->getLabel() == "not" || currentNode->getLabel() == "neg")
    {
        root_node = currentNode;
        //        parentNode->appendChild(currentNode);
    }
    else if (std::find(binaryOperators.begin(), binaryOperators.end(), currentNode->getLabel()) !=
             binaryOperators.end())
    {

        root_node = currentNode;
        //        parentNode->appendChild(currentNode);
    }
    else if (currentNode->getLabel() == "@")
    {
        if (currentNode->getChildCount() == 3)
        {
            std::vector<CustomTreeNode *> children = currentNode->getChildren();

            CustomTreeNode *first_gamma_node = new InternalNode("gamma");
            CustomTreeNode *second_gamma_node = new InternalNode("gamma");

            // Construct the first_gamma_node
            first_gamma_node->appendChild(second_gamma_node);
            first_gamma_node->appendChild(children[2]);

            // Construct the second_gamma_node
            second_gamma_node->appendChild(children[1]);
            second_gamma_node->appendChild(children[0]);

            root_node = first_gamma_node; // Assign the first_gamma_node as the new root node
                                          //        parentNode->appendChild(first_gamma_node);
        }
        else
        {
            throw std::runtime_error("Error: @ node must have 3 children.");
        }
    }
    else if (currentNode->getLabel() == "and")
    {
        if (currentNode->getChildCount() >= 2)
        {
            std::vector<CustomTreeNode *> children = currentNode->getChildren();

            CustomTreeNode *eq_node = new InternalNode("=");
            CustomTreeNode *comma_node = new InternalNode(",");
            CustomTreeNode *tau_node = new InternalNode("tau");

            // Construct the eq_node and its children
            eq_node->appendChild(comma_node);
            eq_node->appendChild(tau_node);

            // Iterate through the children and modify comma_node and tau_node
            for (CustomTreeNode *child : children)
            {
                comma_node->appendChild(child->getChildren()[0]);
                tau_node->appendChild(child->getChildren()[1]);
            }

            root_node = eq_node; // Assign the eq_node as the new root node
                                 //        parentNode->appendChild(eq_node);
        }
        else
        {
            throw std::runtime_error("Error: and node must have at least 2 children.");
        }
    }
    else if (currentNode->getLabel() == "->")
    {
        root_node = currentNode;
        //        parentNode->appendChild(currentNode);
    }
    else if (currentNode->getLabel() == "rec")
    {
        if (currentNode->getChildCount() == 1)
        {
            CustomTreeNode *eq_node = currentNode->getChildren()[0];
            CustomTreeNode *var_node = eq_node->getChildren()[0];
            CustomTreeNode *expr_node = eq_node->getChildren()[1];

            CustomTreeNode *new_eq_node = new InternalNode("=");

            new_eq_node->appendChild(var_node);

            CustomTreeNode *new_gamma_node = new InternalNode("gamma");
            CustomTreeNode *new_lambda_node = new InternalNode("lambda");
            CustomTreeNode *y_str_node = new LeafNode("identifier", "Y*");

            new_gamma_node->appendChild(y_str_node);
            new_gamma_node->appendChild(new_lambda_node);

            new_lambda_node->appendChild(var_node);
            new_lambda_node->appendChild(expr_node);

            new_eq_node->appendChild(new_gamma_node);

            delete eq_node;

            root_node = new_eq_node;
            //        parentNode->appendChild(currentNode);
        }
        else
        {
            throw std::runtime_error("Error: rec node must have 1 child.");
        }
    }
    else
    {
        root_node = currentNode;
        //        parentNode->appendChild(currentNode);
    }
    if (parentNode == nullptr)
    {
        // If the parentNode is null, set the root_node as the new syntax customTree root
        CustomTree::getInstance().setSTRoot(root_node);
        return; // Exit the function
    }
    else
    {
        // If the parentNode is not null, add the root_node as a child of the parentNode
        parentNode->appendChild(root_node);
    }

    if (root_node != currentNode)
    {
        // If the root_node and currentNode are different, it means the currentNode was replaced
        delete currentNode; // Delete the old currentNode to free memory
    }
}

#endif // CUSTOM_TREE_H

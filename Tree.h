#ifndef TREE_H
#define TREE_H

#include "TreeNode.h"

void generateST(TreeNode *currentNode, TreeNode *parentNode);

/**
 * @brief Represents the Tree for a program.
 *
 * The Tree class stores the root nodes of the AST and ST and provides
 * access to them. It follows the Singleton design pattern to ensure
 * that only one instance of the Tree exists.
 */
class Tree
{
private:
    static Tree *tree;           // The singleton instance of the Tree
    TreeNode *astRoot = nullptr; // The root node of the Abstract Syntax Tree (AST)
    TreeNode *stRoot = nullptr;  // The root node of the Standardized Tree (ST)

    Tree() {} // Private constructor

    ~Tree() {} // Private destructor

    Tree(const Tree &) = delete; // Disable copy constructor

    Tree &operator=(const Tree &) = delete; // Disable assignment operator

public:
    /**
     * @brief Returns the singleton instance of the Tree.
     * @return The reference to the Tree instance.
     */
    static Tree &getInstance()
    {
        return *tree;
    }

    /**
     * @brief Sets the root node of the Abstract Syntax Tree (AST).
     * @param r The root node to set.
     */
    void setASTRoot(TreeNode *r)
    {
        astRoot = r;
    }

    /**
     * @brief Retrieves the root node of the Abstract Syntax Tree (AST).
     * @return The root node of the AST.
     */
    TreeNode *getASTRoot()
    {
        return astRoot;
    }

    /**
     * @brief Sets the root node of the Standardized Tree (ST).
     * @param r The root node to set.
     */
    void setSTRoot(TreeNode *r)
    {
        stRoot = r;
    }

    /**
     * @brief Retrieves the root node of the Standardized Tree (ST).
     * @return The root node of the ST.
     */
    TreeNode *getSTRoot()
    {
        return stRoot;
    }

    /**
     * @brief Releases the memory occupied by the Abstract Syntax Tree (AST).
     *
     * This function calls the static releaseNodeMemory() function
     * of the TreeNode class to release the memory of all AST nodes.
     * It should be called when the AST is no longer needed to avoid memory leaks.
     */
    static void releaseASTMemory()
    {
        if (tree->astRoot != nullptr)
        {
            if (tree->stRoot == nullptr)
            {
                tree->stRoot = tree->astRoot;
            }

            tree->astRoot = nullptr;
        }
    }

    /**
     * @brief Releases the memory occupied by the Standardized Tree (ST).
     *
     * This function calls the static releaseNodeMemory() function
     * of the TreeNode class to release the memory of all ST nodes.
     * It should be called when the ST is no longer needed to avoid memory leaks.
     */
    static void releaseSTMemory()
    {
        TreeNode::releaseNodeMemory(tree->stRoot);
    }

    /**
     * @brief Generates the Standardized Tree (ST) from the Abstract Syntax Tree (AST).
     *
     * This function calls the generateST() function to generate the ST from the AST.
     * It should be called when the AST is no longer needed to avoid memory leaks.
     */
    static void generate()
    {
        releaseASTMemory();
        generateST(tree->stRoot, nullptr);
    }
};

Tree *Tree::tree = new Tree(); // Initialize the singleton instance of the Tree

/**
 * Generates the Syntax Tree (ST) by modifying the given tree structure.
 *
 * @param currentNode The current node being processed.
 * @param parentNode The parent node of the current node.
 */
void generateST(TreeNode *currentNode, TreeNode *parentNode)
{
    if (currentNode == nullptr)
    {
        return; // If the current node is nullptr, exit the function
    }

    if (currentNode->getNumChildren() != 0)
    {
        std::vector<TreeNode *> children = currentNode->getChildren(); // Get the children of the current node
        for (TreeNode *child : children)
        {
            generateST(child, currentNode); // Recursively generate the syntax tree for each child
            currentNode->removeChild(0);    // Remove the processed child from the current node
        }
    }

    TreeNode *root_node; // Assign the current node as the root node of the syntax tree

    // Define a static vector of binary operators
    static std::vector<std::string> binaryOperators = {
        "+", "-", "*", "/", "**", "gr", "ge", "ls", "le", "aug", "or", "&", "eq", "ne"};

    if (currentNode->getLabel() == "let")
    {
        if (currentNode->getNumChildren() == 2)
        {
            std::vector<TreeNode *> children = currentNode->getChildren();

            TreeNode *eq_node;
            TreeNode *p_node;

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

            if (eq_node->getNumChildren() == 2)
            {
                TreeNode *lambda_node = new InternalNode("lambda");
                TreeNode *gamma_node = new InternalNode("gamma");

                TreeNode *var_node = eq_node->getChildren()[0];
                TreeNode *expr_node = eq_node->getChildren()[1];

                lambda_node->addChild(var_node);
                lambda_node->addChild(p_node);

                gamma_node->addChild(lambda_node);
                gamma_node->addChild(expr_node);

                root_node = gamma_node; // Assign the gamma_node as the new root node
                                        //            parentNode->addChild(gamma_node);
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
        if (currentNode->getNumChildren() == 2)
        {
            std::vector<TreeNode *> children = currentNode->getChildren();

            TreeNode *eq_node;
            TreeNode *p_node;

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

            if (eq_node->getNumChildren() == 2)
            {
                TreeNode *lambda_node = new InternalNode("lambda");
                TreeNode *gamma_node = new InternalNode("gamma");

                TreeNode *var_node = eq_node->getChildren()[0];
                TreeNode *expr_node = eq_node->getChildren()[1];

                lambda_node->addChild(var_node);
                lambda_node->addChild(p_node);

                gamma_node->addChild(lambda_node);
                gamma_node->addChild(expr_node);

                root_node = gamma_node; // Assign the gamma_node as the new root node
                                        //            parentNode->addChild(gamma_node);
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
        if (currentNode->getNumChildren() > 2)
        {
            std::vector<TreeNode *> children = currentNode->getChildren();

            TreeNode *fcn_name_node = children.front();
            // Remove fcn_name_node from children
            children.erase(children.begin());

            TreeNode *expr_node = children.back();
            // Remove expr_node from children
            children.pop_back();

            TreeNode *eq_node = new InternalNode("=");

            eq_node->addChild(fcn_name_node);

            TreeNode *prev_node = eq_node;
            for (TreeNode *child : children)
            {
                TreeNode *lambda_node = new InternalNode("lambda");
                lambda_node->addChild(child);
                prev_node->addChild(lambda_node);
                prev_node = lambda_node;
            }

            prev_node->addChild(expr_node);

            root_node = eq_node; // Assign the eq_node as the new root node
                                 //        parentNode->addChild(eq_node);
        }
        else
        {
            throw std::runtime_error("Error: fcn_form node must have more than 2 children.");
        }
    }
    else if (currentNode->getLabel() == "tau")
    {
        // if (currentNode->getNumChildren() >= 2)
        // {
        //     TreeNode *head_first_gamma_node = new InternalNode("gamma");
        //     TreeNode *head_second_gamma_node = new InternalNode("gamma");
        //     head_first_gamma_node->addChild(head_second_gamma_node);

        //     TreeNode *aug_node = new LeafNode("identifier", "aug");
        //     head_second_gamma_node->addChild(aug_node);

        //     std::vector<TreeNode *> children = currentNode->getChildren();
        //     // reverse the children vector
        //     std::reverse(children.begin(), children.end());

        //     TreeNode *prev_first_node = head_first_gamma_node;
        //     TreeNode *prev_second_node = head_second_gamma_node;
        //     for (TreeNode *child : children)
        //     {
        //         prev_first_node->addChild(child);

        //         TreeNode *new_first_gamma_node = new InternalNode("gamma");
        //         TreeNode *new_second_gamma_node = new InternalNode("gamma");
        //         new_first_gamma_node->addChild(new_second_gamma_node);

        //         TreeNode *new_aug_node = new LeafNode("identifier", "aug");
        //         new_second_gamma_node->addChild(new_aug_node);

        //         prev_second_node->addChild(new_first_gamma_node);

        //         prev_first_node = new_first_gamma_node;
        //         prev_second_node = new_second_gamma_node;
        //     }

        //     TreeNode *nil_node = new LeafNode("identifier", "nil");
        //     prev_second_node->addChild(nil_node);

        //     parentNode->addChild(head_first_gamma_node);
        // }
        // else
        // {
        //     std::runtime_error("Error: tau node must have at least 2 children.");
        // }

        root_node = currentNode;
        //        parentNode->addChild(currentNode);
    }
    else if (currentNode->getLabel() == "lambda" && currentNode->getChildren()[0]->getLabel() != "," &&
             currentNode->getChildren()[1]->getLabel() != ",")
    {
        if (currentNode->getNumChildren() >= 2)
        {
            std::vector<TreeNode *> children = currentNode->getChildren();

            TreeNode *expr_node = children.back();
            // Remove expr_node from children
            children.pop_back();

            TreeNode *head_lambda_node = new InternalNode("lambda");

            TreeNode *prev_node = head_lambda_node;
            for (TreeNode *child : children)
            {
                TreeNode *lambda_node = new InternalNode("lambda");
                lambda_node->addChild(child);
                prev_node->addChild(lambda_node);
                prev_node = lambda_node;
            }

            prev_node->addChild(expr_node);

            root_node = head_lambda_node->getChildren()[0];
            head_lambda_node->getChildren().erase(head_lambda_node->getChildren().begin());
            delete head_lambda_node;
            //        parentNode->addChild(head_lambda_node);
        }
        else
        {
            throw std::runtime_error("Error: lambda node must have at least 2 children.");
        }
    }
    else if (currentNode->getLabel() == "within")
    {
        if (currentNode->getNumChildren() == 2)
        {
            std::vector<TreeNode *> children = currentNode->getChildren();

            // Check if each child is the "=" node and has exactly 2 children
            for (TreeNode *child : children)
            {
                if (child->getLabel() != "=")
                {
                    throw std::runtime_error("Error: within node must have an = node as a child");
                }
                else if (child->getNumChildren() != 2)
                {
                    throw std::runtime_error("Error: = node must have 2 children.");
                }
            }

            // Retrieve the two "=" nodes
            TreeNode *first_eq_node = children[0];
            TreeNode *second_eq_node = children[1];

            // Create new nodes for constructing the modified syntax tree
            TreeNode *new_eq_node = new InternalNode("=");
            TreeNode *new_gamma_node = new InternalNode("gamma");
            TreeNode *new_lambda_node = new InternalNode("lambda");

            // Modify the new_eq_node and new_gamma_node
            new_eq_node->addChild(second_eq_node->getChildren()[0]);
            new_eq_node->addChild(new_gamma_node);

            // Modify the new_gamma_node
            new_gamma_node->addChild(new_lambda_node);
            new_gamma_node->addChild(first_eq_node->getChildren()[1]);

            // Modify the new_lambda_node
            new_lambda_node->addChild(first_eq_node->getChildren()[0]);
            new_lambda_node->addChild(second_eq_node->getChildren()[1]);

            root_node = new_eq_node; // Assign the new_eq_node as the new root node
                                     //        parentNode->addChild(new_eq_node);
        }
        else
        {
            throw std::runtime_error("Error: within node must have 2 children.");
        }
    }
    else if (currentNode->getLabel() == "not" || currentNode->getLabel() == "neg")
    {
        // TreeNode *new_gamma_node = new InternalNode("gamma");
        // TreeNode *new_uop_node = new LeafNode("identifier", currentNode->getLabel());

        // new_gamma_node->addChild(new_uop_node);
        // new_gamma_node->addChild(currentNode->getChildren()[0]);

        // parentNode->addChild(new_gamma_node);

        root_node = currentNode;
        //        parentNode->addChild(currentNode);
    }
    else if (std::find(binaryOperators.begin(), binaryOperators.end(), currentNode->getLabel()) !=
             binaryOperators.end())
    {
        // if (currentNode->getNumChildren() == 2)
        // {
        //     std::vector<TreeNode *> children = currentNode->getChildren();

        //     TreeNode *first_gamma_node = new InternalNode("gamma");
        //     TreeNode *second_gamma_node = new InternalNode("gamma");

        //     first_gamma_node->addChild(second_gamma_node);
        //     first_gamma_node->addChild(children[1]);

        //     TreeNode *new_binop_node = new LeafNode("identifier", currentNode->getLabel());

        //     second_gamma_node->addChild(new_binop_node);
        //     second_gamma_node->addChild(children[0]);

        //     parentNode->addChild(first_gamma_node);
        // } else {
        //     std::runtime_error("Error: binary operator node must have 2 children.");
        // }

        root_node = currentNode;
        //        parentNode->addChild(currentNode);
    }
    else if (currentNode->getLabel() == "@")
    {
        if (currentNode->getNumChildren() == 3)
        {
            std::vector<TreeNode *> children = currentNode->getChildren();

            TreeNode *first_gamma_node = new InternalNode("gamma");
            TreeNode *second_gamma_node = new InternalNode("gamma");

            // Construct the first_gamma_node
            first_gamma_node->addChild(second_gamma_node);
            first_gamma_node->addChild(children[2]);

            // Construct the second_gamma_node
            second_gamma_node->addChild(children[1]);
            second_gamma_node->addChild(children[0]);

            root_node = first_gamma_node; // Assign the first_gamma_node as the new root node
                                          //        parentNode->addChild(first_gamma_node);
        }
        else
        {
            throw std::runtime_error("Error: @ node must have 3 children.");
        }
    }
    else if (currentNode->getLabel() == "and")
    {
        if (currentNode->getNumChildren() >= 2)
        {
            std::vector<TreeNode *> children = currentNode->getChildren();

            TreeNode *eq_node = new InternalNode("=");
            TreeNode *comma_node = new InternalNode(",");
            TreeNode *tau_node = new InternalNode("tau");

            // Construct the eq_node and its children
            eq_node->addChild(comma_node);
            eq_node->addChild(tau_node);

            // Iterate through the children and modify comma_node and tau_node
            for (TreeNode *child : children)
            {
                comma_node->addChild(child->getChildren()[0]);
                tau_node->addChild(child->getChildren()[1]);
            }

            root_node = eq_node; // Assign the eq_node as the new root node
                                 //        parentNode->addChild(eq_node);
        }
        else
        {
            throw std::runtime_error("Error: and node must have at least 2 children.");
        }
    }
    else if (currentNode->getLabel() == "->")
    {
        root_node = currentNode;
        //        parentNode->addChild(currentNode);
    }
    else if (currentNode->getLabel() == "rec")
    {
        if (currentNode->getNumChildren() == 1)
        {
            TreeNode *eq_node = currentNode->getChildren()[0];
            TreeNode *var_node = eq_node->getChildren()[0];
            TreeNode *expr_node = eq_node->getChildren()[1];

            TreeNode *new_eq_node = new InternalNode("=");

            new_eq_node->addChild(var_node);

            TreeNode *new_gamma_node = new InternalNode("gamma");
            TreeNode *new_lambda_node = new InternalNode("lambda");
            TreeNode *y_str_node = new LeafNode("identifier", "Y*");

            new_gamma_node->addChild(y_str_node);
            new_gamma_node->addChild(new_lambda_node);

            new_lambda_node->addChild(var_node);
            new_lambda_node->addChild(expr_node);

            new_eq_node->addChild(new_gamma_node);

            delete eq_node;

            root_node = new_eq_node;
            //        parentNode->addChild(currentNode);
        }
        else
        {
            throw std::runtime_error("Error: rec node must have 1 child.");
        }
    }
    else
    {
        root_node = currentNode;
        //        parentNode->addChild(currentNode);
    }
    if (parentNode == nullptr)
    {
        // If the parentNode is null, set the root_node as the new syntax tree root
        Tree::getInstance().setSTRoot(root_node);
        return; // Exit the function
    }
    else
    {
        // If the parentNode is not null, add the root_node as a child of the parentNode
        parentNode->addChild(root_node);
    }

    if (root_node != currentNode)
    {
        // If the root_node and currentNode are different, it means the currentNode was replaced
        delete currentNode; // Delete the old currentNode to free memory
    }
}

#endif // TREE_H

#ifndef TREENODE_H
#define TREENODE_H

#include <string>
#include <utility>
#include <vector>
#include <algorithm>

/**
 * @brief Represents a node in a tree structure.
 *
 * The TreeNode class represents a node in a tree. It contains a label,
 * a vector of child nodes, and an optional value. It provides methods
 * to add children, reverse the order of children, and retrieve information
 * about the node.
 */
class TreeNode
{
private:
    std::string label;                // The label of the node
    std::vector<TreeNode *> children; // The child nodes of the current node
    std::string value;                // The value associated with the node

public:
    /**
     * @brief Constructs a TreeNode object with the specified label.
     * @param l The label of the node.
     */
    explicit TreeNode(std::string l) : label(std::move(l))
    {
        children = std::vector<TreeNode *>();
    }

    /**
     * @brief Adds a child node to the current node.
     * @param child The child node to add.
     */
    void addChild(TreeNode *child)
    {
        children.push_back(child);
    }

    /**
     * @brief Reverses the order of the child nodes.
     */
    void reverseChildren()
    {
        std::reverse(children.begin(), children.end());
    }

    /**
     * @brief Removes a child node from the current node.
     * @param index The index of the child node to remove.
     * @param deleteNode Whether or not to delete the node.
     */
    void removeChild(int index = 0, bool deleteNode = false)
    {
        if (index < 0 || index >= children.size())
        {
            throw std::out_of_range("Index out of range");
        }

        if (deleteNode)
        {
            releaseNodeMemory(children[index]);
        }

        children.erase(children.begin() + index);
    }

    /**
     * @brief Returns the number of child nodes.
     * @return The number of child nodes.
     */
    int getNumChildren()
    {
        return children.size();
    }

    /**
     * @brief Returns the label of the node.
     * @return The label of the node as a string.
     */
    std::string getLabel()
    {
        return label;
    }

    /**
     * @brief Returns a reference to the vector of child nodes.
     * @return A reference to the vector of child nodes.
     */
    std::vector<TreeNode *> &getChildren()
    {
        return children;
    }

    /**
     * @brief Returns the value associated with the node.
     * @return The value of the node as a string.
     */
    virtual std::string getValue()
    {
        return value;
    }

    /**
     * @brief Sets the value associated with the node.
     * @param v The value to set.
     */
    void setValue(std::string v)
    {
        value = std::move(v);
    }

    /**
     * @brief Releases the memory occupied by a TreeNode and its child nodes.
     * @param node The node to release memory for.
     */
    static void releaseNodeMemory(TreeNode *node)
    {
        if (node == nullptr)
            return;

        for (TreeNode *child : node->getChildren())
        {
            releaseNodeMemory(child);
        }

        delete node;
    }
};

/**
 * @brief Represents an internal node in a tree structure.
 *
 * The InternalNode class is a specialization of the TreeNode class
 * that represents an internal node in a tree. It has a label and an
 * optional value associated with it.
 */
class InternalNode : public TreeNode
{
public:
    /**
     * @brief Constructs an InternalNode object with the specified label and value.
     * @param l The label of the internal node.
     * @param v The value associated with the internal node (default: "").
     */
    InternalNode(const std::string &l, const std::string &v = " ") : TreeNode(l)
    {
        setValue(" ");
    }
};

/**
 * @brief Represents a leaf node in a tree structure.
 *
 * The LeafNode class is a specialization of the TreeNode class that
 * represents a leaf node in a tree. It has a label and a value associated
 * with it.
 */
class LeafNode : public TreeNode
{
public:
    /**
     * @brief Constructs a LeafNode object with the specified label and value.
     * @param l The label of the leaf node.
     * @param v The value associated with the leaf node.
     */
    LeafNode(const std::string &l, const std::string &v) : TreeNode(l)
    {
        setValue(v);
    }

    void addChild(TreeNode *child) = delete;
};

#endif // TREENODE_H

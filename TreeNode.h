#ifndef CUSTOM_TREE_NODE_H
#define CUSTOM_TREE_NODE_H

#include <string>
#include <vector>
#include <algorithm>

/**
 * @brief Represents a custom tree node.
 *
 * The CustomTreeNode class represents a node in a custom tree structure.
 * It contains a label, a vector of child nodes, and an optional value.
 * It provides methods to add children, reverse the order of children, and
 * retrieve information about the node.
 */
class CustomTreeNode
{
private:
    std::string nodeLabel;                // The nodeLabel of the node
    std::vector<CustomTreeNode *> children; // The child nodes of the current node
    std::string nodeValue;                // The nodeValue associated with the node

public:
    /**
     * @brief Constructs a CustomTreeNode object with the specified label.
     * @param label The label of the node.
     */
    explicit CustomTreeNode(std::string l) : nodeLabel(std::move(l))
    {
        children = std::vector<CustomTreeNode *>();
    }

    /**
     * @brief Adds a child node to the current node.
     * @param child The child node to add.
     */
    void appendChild(CustomTreeNode *child)
    {
        children.push_back(child);
    }

    /**
     * @brief Reverses the order of the child nodes.
     */
    void reverseChildrenOrder()
    {
        std::reverse(children.begin(), children.end());
    }
    
    /**
     * @brief Returns the number of child nodes.
     * @return The number of child nodes.
     */
    int getChildCount()
    {
        return children.size();
    }
        /**
     * @brief Returns the nodeLabel of the node.
     * @return The nodeLabel of the node as a string.
     */
    std::string getLabel() const
    {
        return nodeLabel;
    }
        /**
     * @brief Returns a reference to the vector of child nodes.
     * @return A reference to the vector of child nodes.
     */
    std::vector<CustomTreeNode *> & getChildren()
    {
        return children;
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
            deleteNodeMemory(children[index]);
        }

        children.erase(children.begin() + index);
    }
    /**
     * @brief Returns the value associated with the node.
     * @return The value of the node as a string.
     */
    virtual std::string getValue() const
    {
        return nodeValue;
    }

    /**
     * @brief Sets the value associated with the node.
     * @param value The value to set.
     */
    void setValue(std::string value)
    {
        nodeValue = std::move(value);
    }

    /**
     * @brief Releases the memory occupied by a CustomTreeNode and its child nodes.
     * @param node The node to release memory for.
     */
    static void deleteNodeMemory(CustomTreeNode *node)
    {
        if (node == nullptr)
            return;

        for (CustomTreeNode *child : node->getChildren())
        {
            deleteNodeMemory(child);
        }

        delete node;
    }
};

/**
 * @brief Represents an internal node in a custom tree structure.
 */
class InternalNode : public CustomTreeNode
{
public:
    /**
     * @brief Constructs an InternalNode object with the specified nodeLabel and nodeValue.
     * @param l The nodeLabel of the internal node.
     * @param v The nodeValue associated with the internal node.
     */
    InternalNode(const std::string &l, const std::string &v = " ") : CustomTreeNode(l)
    {
        setValue(" ");
    }
};

/**
 * @brief Represents a leaf node in a custom tree structure.
 */
class LeafNode : public CustomTreeNode
{
public:
    /**
     * @brief Constructs a LeafNode object with the specified nodeLabel and nodeValue.
     * @param l The nodeLabel of the leaf node.
     * @param v The nodeValue associated with the leaf node.
     */
    LeafNode(const std::string &l, const std::string &v) : CustomTreeNode(l)
    {
        setValue(v);
    }

    void appendChild(CustomTreeNode *child) = delete;
};

#endif // CUSTOM_TREE_NODE_H

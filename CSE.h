#ifndef CSE_H
#define CSE_H

#include <utility>
#include <vector>
#include <unordered_map>
#include <string>
#include <stdexcept>

#include "Tree.h"

// Types of nodes are enumerated for CSE machine
enum class TypeOfObject : int
{
    LAMBDA,
    IDENTIFIER,
    INTEGER,
    STRING,
    GAMMA,
    OPERATOR,
    BETA,
    EETA,
    DELTA,
    TAU,
    ENVIRONMENT,
    LIST,
    BOOLEAN
};

std::vector<std::string> builtInFunctions = {"Print", "print", "Order", "Y*", "Conc", "Stem", "Stern", "Isinteger", "Isstring",
                                             "Istuple", "Isempty", "dummy", "ItoS"};

bool is_operator(const std::string &labelOfNode);

class CseNode
{
private:
    // General node properties
    TypeOfObject typeOfNode;
    std::string valueOfNode;

    // CseNode properties for lambda and eeta nodes
    int environment{};
    int csIndex{}; // for delta, tau, eeta, lambda nodes
    std::vector<std::string> boundVariables;
    std::vector<CseNode> listOfElements;
    bool isOneBoundVariable = true;

public:
    CseNode() = default;

    // Constructor for lambda (in stack) and eeta nodes
    CseNode(TypeOfObject typeOfNode, std::string valueOfNode, int csIndex, int environment)
    {
        this->typeOfNode = typeOfNode;
        this->valueOfNode = std::move(valueOfNode);
        this->csIndex = csIndex;
        this->environment = environment;
    }

    // Constructor for lambda (in control structure) nodes
    CseNode(TypeOfObject typeOfNode, std::string valueOfNode, int csIndex)
    {
        this->typeOfNode = typeOfNode;
        this->valueOfNode = std::move(valueOfNode);
        this->csIndex = csIndex;
    }

    // Constructor for other nodes
    CseNode(TypeOfObject typeOfNode, std::string valueOfNode)
    {
        this->typeOfNode = typeOfNode;
        this->valueOfNode = std::move(valueOfNode);
    }

    // Constructor for lambda (in cs) nodes with bound variables
    CseNode(TypeOfObject typeOfNode, int csIndex, std::vector<std::string> boundVariables)
    {
        isOneBoundVariable = false;
        this->typeOfNode = typeOfNode;
        this->csIndex = csIndex;
        this->boundVariables = std::move(boundVariables);
    }

    // Constructor for lambda (in stack) nodes with bound variables
    CseNode(TypeOfObject typeOfNode, int csIndex, std::vector<std::string> boundVariables, int environment)
    {
        isOneBoundVariable = false;
        this->typeOfNode = typeOfNode;
        this->valueOfNode = std::move(valueOfNode);
        this->csIndex = csIndex;
        this->environment = environment;
        this->boundVariables = std::move(boundVariables);
    }

    CseNode(TypeOfObject typeOfNode, std::vector<CseNode> listOfElements)
    {
        this->typeOfNode = typeOfNode;
        this->listOfElements = std::move(listOfElements);
    }

    // Getters
    TypeOfObject get_type_of_node() const
    {
        return typeOfNode;
    }

    std::string get_value_of_node() const
    {
        return valueOfNode;
    }

    int get_environment() const
    {
        return environment;
    }

    int get_cs_index() const
    {
        return csIndex;
    }

    bool get_is_one_bound_var() const
    {
        return isOneBoundVariable;
    }

    std::vector<std::string> get_bound_variables_list() const
    {
        return boundVariables;
    }

    std::vector<CseNode> get_list_elements() const
    {
        return listOfElements;
    }

    CseNode set_env(int environment_)
    {
        this->environment = environment_;
        return *this;
    }
};

class ControlStructure
{
private:
    int csIndex;
    std::vector<CseNode> nodes;

public:
    // Constructor with empty nodes
    explicit ControlStructure(int csIndex)
    {
        this->csIndex = csIndex;
    }

    // add node to control structure
    void append_node(CseNode node)
    {
        nodes.push_back(node);
    }

    // Getters
    int get_cs_index() const
    {
        return csIndex;
    }

    // return the last node in the control structure
    CseNode get_last_node() const
    {
        return nodes.back();
    }

    // pop the last node in the control structure
    void pop_last_node()
    {
        nodes.pop_back();
    }

    // pop and return the last node in the control structure
    CseNode pop_last_node_return()
    {
        CseNode node = nodes.back();
        nodes.pop_back();
        return node;
    }

    // push another control structure to the current control structure
    void push_cs(const ControlStructure &cs)
    {
        for (auto &node : cs.nodes)
        {
            nodes.push_back(node);
        }
    }
};

class Stack
{
private:
    std::vector<CseNode> nodes;

public:
    // constructor with empty nodes
    Stack() = default;

    // add node to stack
    void append_node(const CseNode &node)
    {
        nodes.push_back(node);
    }

    // pop the last node in the stack
    void pop_last_node()
    {
        nodes.pop_back();
    }

    // pop and return the last node in the stack
    CseNode pop_last_node_return()
    {
        CseNode node = nodes.back();
        nodes.pop_back();
        return node;
    }

    // length of the stack
    int length() const
    {
        return static_cast<int>(nodes.size());
    }
};

class Environment
{
private:
    std::unordered_map<std::string, CseNode> variables;
    std::unordered_map<std::string, CseNode> lambdas;
    std::unordered_map<std::string, std::vector<CseNode>> lists;
    bool isLambda = false;
    Environment *parentEnvironment;

public:
    // constructor with empty variables and lambdas
    Environment()
    {
        parentEnvironment = nullptr;
    }

    // constructor with empty variables and lambdas
    explicit Environment(Environment *parentEnvironment)
    {
        this->parentEnvironment = parentEnvironment;
    }

    // add variable to environment
    void append_variable(const std::string &identifier, const CseNode &nodeValue)
    {
        variables[identifier] = nodeValue;
    }

    void append_variables(const std::vector<std::string> &identifiers,
                          const std::vector<CseNode> &values)
    {
        for (int i = 0; i < identifiers.size(); i++)
        {
            variables[identifiers[i]] = values[i];
        }
    }

    void append_list(const std::string &identifier, std::vector<CseNode> listOfElements)
    {
        lists[identifier] = std::move(listOfElements);
    }

    // add lambda to environment
    void append_lambda(const std::string &identifier, const CseNode &lambda)
    {
        isLambda = true;

        // check the node type and create new object
        if (lambda.get_type_of_node() == TypeOfObject::LAMBDA)
        {
            if (lambda.get_is_one_bound_var())
            {
                lambdas[identifier] = CseNode(TypeOfObject::LAMBDA,
                                              lambda.get_value_of_node(), lambda.get_cs_index(), lambda.get_environment());
            }
            else
            {
                lambdas[identifier] = CseNode(TypeOfObject::LAMBDA, lambda.get_cs_index(),
                                              lambda.get_bound_variables_list(), lambda.get_environment());
            }
        }
        else if (lambda.get_type_of_node() == TypeOfObject::EETA)
        {
            if (lambda.get_is_one_bound_var())
            {
                lambdas[identifier] = CseNode(TypeOfObject::EETA,
                                              lambda.get_value_of_node(), lambda.get_cs_index(), lambda.get_environment());
            }
            else
            {
                lambdas[identifier] = CseNode(TypeOfObject::EETA, lambda.get_cs_index(),
                                              lambda.get_bound_variables_list(), lambda.get_environment());
            }
        }
        else
        {
            throw std::runtime_error("Invalid lambda node type");
        }
    }

    // get variable from environment
    CseNode get_variable(const std::string &identifier)
    {
        if (variables.find(identifier) != variables.end())
        {
            return variables[identifier];
        }
        else if (parentEnvironment != nullptr)
        {
            return parentEnvironment->get_variable(identifier);
        }
        else
        {
            throw std::runtime_error("Identifier: " + identifier + " is not found");
        }
    }

    // get lambda from environment
    CseNode get_lambda(const std::string &identifier)
    {
        if (lambdas.find(identifier) != lambdas.end())
        {
            return lambdas[identifier];
        }
        else if (parentEnvironment != nullptr)
        {
            return parentEnvironment->get_lambda(identifier);
        }
        else
        {
            throw std::runtime_error("Identifier: " + identifier + " is not found");
        }
    }

    // get list from environment
    std::vector<CseNode> get_list(const std::string &identifier)
    {
        if (lists.find(identifier) != lists.end())
        {
            return lists[identifier];
        }
        else if (parentEnvironment != nullptr)
        {
            return parentEnvironment->get_list(identifier);
        }
        else
        {
            throw std::runtime_error("Identifier: " + identifier + " is not found");
        }
    }
};

class CSE
{
private:
    int nextEnvironment = 0;
    int nextCS = -1;

    std::vector<ControlStructure *> ControlStructs;
    ControlStructure main_cs = ControlStructure(-1);
    Stack stack = Stack();
    std::vector<int> environment_stack = std::vector<int>();
    std::unordered_map<int, Environment *> environments = std::unordered_map<int, Environment *>();

public:
    // constructor with empty control structures and stack
    CSE() = default;

    // create control structures
    void create_cs(CustomTreeNode *root, ControlStructure *current_cs = nullptr, int current_cs_index = -1)
    {
        ControlStructure *cs;

        if (nextCS == -1)
        {
            nextCS++;
            cs = new ControlStructure(nextCS++);
            ControlStructs.push_back(cs);
            current_cs_index = 0;
        }
        else
        {
            cs = current_cs;
        }

        if (root->getLabel() == "lambda")
        {
            CseNode *lambda;
            if (root->getChildren()[0]->getLabel() == ",")
            {
                std::vector<std::string> vars;
                for (auto &child : root->getChildren()[0]->getChildren())
                {
                    vars.push_back(child->getValue());
                }
                lambda = new CseNode(TypeOfObject::LAMBDA, nextCS, vars);
            }
            else
            {
                std::string var = root->getChildren()[0]->getValue();
                lambda = new CseNode(TypeOfObject::LAMBDA, var, nextCS);
            }

            cs->append_node(*lambda);

            auto *new_cs = new ControlStructure(nextCS);
            ControlStructs.push_back(new_cs);
            create_cs(root->getChildren()[1], new_cs, nextCS++);
        }
        else if (root->getLabel() == "tau")
        {
            auto *tau = new CseNode(TypeOfObject::TAU, std::to_string(root->getChildren().size()));
            cs->append_node(*tau);

            for (auto &child : root->getChildren())
            {
                create_cs(child, cs, current_cs_index);
            }
        }
        else if (root->getLabel() == "->")
        {
            int then_index = nextCS++;
            int else_index = nextCS++;
            auto *delta_then = new CseNode(TypeOfObject::DELTA, std::to_string(then_index));
            auto *delta_else = new CseNode(TypeOfObject::DELTA, std::to_string(else_index));
            auto *beta = new CseNode(TypeOfObject::BETA, "");

            cs->append_node(*delta_then);
            cs->append_node(*delta_else);
            cs->append_node(*beta);

            auto *then_cs = new ControlStructure(then_index);
            auto *else_cs = new ControlStructure(else_index);

            ControlStructs.push_back(then_cs);
            ControlStructs.push_back(else_cs);

            create_cs(root->getChildren()[1], then_cs, then_index);
            create_cs(root->getChildren()[2], else_cs, else_index);

            create_cs(root->getChildren()[0], cs, current_cs_index);
        }
        else if (is_operator(root->getLabel()))
        {
            auto *op = new CseNode(TypeOfObject::OPERATOR, root->getLabel());
            cs->append_node(*op);

            for (auto &child : root->getChildren())
            {
                create_cs(child, cs, current_cs_index);
            }
        }
        else if (root->getLabel() == "gamma")
        {
            auto *gamma = new CseNode(TypeOfObject::GAMMA, "");
            cs->append_node(*gamma);

            for (auto &child : root->getChildren())
            {
                create_cs(child, cs, current_cs_index);
            }
        }
        else if (root->getLabel() == "identifier" || root->getLabel() == "integer" || root->getLabel() == "string")
        {
            std::string value = root->getValue();
            std::string type = root->getLabel();
            CseNode *leaf;

            if (type == "identifier")
            {
                leaf = new CseNode(TypeOfObject::IDENTIFIER, value);
            }
            else if (type == "integer")
            {
                leaf = new CseNode(TypeOfObject::INTEGER, value);
            }
            else if (type == "string")
            {
                leaf = new CseNode(TypeOfObject::STRING, value);
            }
            else
            {
                throw std::runtime_error("Invalid leaf type: " + type);
            }

            cs->append_node(*leaf);
        }
        else
        {
            throw std::runtime_error("Invalid node type: " + root->getLabel() + "Value: " + root->getValue());
        }
    }

    void evaluate()
    {
        auto *e0 = new CseNode(TypeOfObject::ENVIRONMENT, "0");
        main_cs.append_node(*e0);
        stack.append_node(*e0);
        environment_stack.push_back(nextEnvironment++);
        environments[0] = new Environment(nullptr);

        main_cs.push_cs(*ControlStructs[0]);

        CseNode top_of_cs = main_cs.pop_last_node_return();

        while ((top_of_cs.get_type_of_node() != TypeOfObject::ENVIRONMENT) || (top_of_cs.get_value_of_node() != "0"))
        {
            if (top_of_cs.get_type_of_node() == TypeOfObject::INTEGER || top_of_cs.get_type_of_node() == TypeOfObject::STRING)
            {
                stack.append_node(top_of_cs);
                top_of_cs = main_cs.pop_last_node_return();
            }
            else if (top_of_cs.get_type_of_node() == TypeOfObject::IDENTIFIER)
            {
                CseNode nodeValue;
                CseNode value_l;
                std::vector<CseNode> list;

                try
                {
                    nodeValue = environments[environment_stack.back()]->get_variable(top_of_cs.get_value_of_node());
                    stack.append_node(CseNode(nodeValue.get_type_of_node(), nodeValue.get_value_of_node()));
                }
                catch (std::runtime_error &e)
                {
                    try
                    {
                        value_l = environments[environment_stack.back()]->get_lambda(top_of_cs.get_value_of_node());
                        stack.append_node(value_l);
                    }
                    catch (std::runtime_error &e)
                    {
                        try
                        {
                            list = environments[environment_stack.back()]->get_list(top_of_cs.get_value_of_node());
                            stack.append_node(CseNode(TypeOfObject::LIST, list));
                        }
                        catch (std::runtime_error &e)
                        {
                            // if node nodeValue is in builtInFunctions add the node to the stack
                            if (std::find(builtInFunctions.begin(), builtInFunctions.end(),
                                          top_of_cs.get_value_of_node()) !=
                                builtInFunctions.end())
                            {
                                stack.append_node(top_of_cs);
                            }
                            else if (top_of_cs.get_value_of_node() == "nil")
                            {
                                stack.append_node(CseNode(TypeOfObject::LIST, std::vector<CseNode>()));
                            }
                            else
                            {
                                throw std::runtime_error("Variable not found: " + top_of_cs.get_value_of_node());
                            }
                        }
                    }
                }

                top_of_cs = main_cs.pop_last_node_return();
            }
            else if (top_of_cs.get_type_of_node() == TypeOfObject::LAMBDA)
            {
                int current_env = environment_stack.back();
                stack.append_node(top_of_cs.set_env(current_env));

                top_of_cs = main_cs.pop_last_node_return();
            }
            else if (top_of_cs.get_type_of_node() == TypeOfObject::GAMMA)
            {
                CseNode top_of_stack = stack.pop_last_node_return();

                if (top_of_stack.get_type_of_node() == TypeOfObject::LAMBDA)
                {
                    Environment *new_environment = new Environment(environments[top_of_stack.get_environment()]);
                    environments[nextEnvironment++] = new_environment;

                    CseNode nodeValue = stack.pop_last_node_return();

                    if (nodeValue.get_type_of_node() == TypeOfObject::LAMBDA || nodeValue.get_type_of_node() == TypeOfObject::EETA)
                    {
                        new_environment->append_lambda(top_of_stack.get_value_of_node(), nodeValue);
                    }
                    else if (nodeValue.get_type_of_node() == TypeOfObject::STRING || nodeValue.get_type_of_node() == TypeOfObject::INTEGER)
                    {
                        new_environment->append_variable(top_of_stack.get_value_of_node(), nodeValue);
                    }
                    else if (nodeValue.get_type_of_node() == TypeOfObject::LIST && !top_of_stack.get_is_one_bound_var())
                    {

                        std::vector<std::string> variable_list = top_of_stack.get_bound_variables_list();
                        std::vector<CseNode> list_items = nodeValue.get_list_elements();

                        std::vector<CseNode> temporary_list = std::vector<CseNode>();

                        int variable_count = 0;

                        int list_element_count = 0;
                        bool create_list = false;

                        for (const auto &i : list_items)
                        {
                            if (create_list)
                            {
                                temporary_list.push_back(i);
                                list_element_count--;

                                if (list_element_count == 0)
                                {
                                    new_environment->append_list(variable_list[variable_count++], temporary_list);
                                    temporary_list = std::vector<CseNode>();
                                    create_list = false;
                                }
                            }
                            else
                            {
                                if (i.get_type_of_node() == TypeOfObject::LIST)
                                {
                                    list_element_count = std::stoi(i.get_value_of_node());
                                    if (list_element_count == 0)
                                    {
                                        new_environment->append_list(variable_list[variable_count++], temporary_list);
                                        temporary_list = std::vector<CseNode>();
                                    }
                                    else
                                    {
                                        create_list = true;
                                    }
                                }
                                else if (i.get_type_of_node() == TypeOfObject::LAMBDA)
                                {
                                    new_environment->append_lambda(variable_list[variable_count++], i);
                                }
                                else
                                {
                                    new_environment->append_variable(variable_list[variable_count++], i);
                                }
                            }
                        }

                        if (create_list)
                        {
                            new_environment->append_list(variable_list[variable_count], temporary_list);
                        }
                    }
                    else if (nodeValue.get_type_of_node() == TypeOfObject::LIST)
                    {
                        new_environment->append_list(top_of_stack.get_value_of_node(), nodeValue.get_list_elements());
                    }
                    else
                    {
                        throw std::runtime_error("Invalid object for gamma: " + nodeValue.get_value_of_node());
                    }

                    environment_stack.push_back(nextEnvironment - 1);
                    auto *environment_obj = new CseNode(TypeOfObject::ENVIRONMENT, std::to_string(nextEnvironment - 1));
                    main_cs.append_node(*environment_obj);
                    stack.append_node(*environment_obj);
                    main_cs.push_cs(*ControlStructs[top_of_stack.get_cs_index()]);
                }
                else if (top_of_stack.get_type_of_node() == TypeOfObject::IDENTIFIER)
                {

                    std::string identifier = top_of_stack.get_value_of_node();

                    if (identifier == "Print" || identifier == "print") {
                    CseNode value = stack.pop_last_node_return();
                    std::vector<CseNode> list_elements = value.get_list_elements();

                    if (value.get_type_of_node() == TypeOfObject::LIST) {
                        std::cout << "(";

                        std::vector<int> count_stack;

                        for (int i = 0; i < value.get_list_elements().size(); i++) {
                            if (list_elements[i].get_type_of_node() == TypeOfObject::LIST) {
                                if (std::stoi(list_elements[i].get_value_of_node()) == 0) {
                                    if (i == list_elements.size() - 1) {
                                        std::cout << "nil";
                                    } else {
                                        std::cout << "nil, ";
                                    }
                                } else {
                                    for (int &count: count_stack) {
                                        count--;
                                    }

                                    count_stack.push_back(std::stoi(list_elements[i].get_value_of_node()));
                                    std::cout << "(";
                                }
                            } else {
                                std::cout << list_elements[i].get_value_of_node();

                                if (!count_stack.empty()) {
                                    // reduce 1 from all elements in count_stack
                                    for (int &count: count_stack) {
                                        count--;
                                    }

                                    if (count_stack[count_stack.size() - 1] == 0) {
                                        int finished_lists = 0;

                                        while (count_stack[count_stack.size() - 1] == 0) {
                                            finished_lists++;
                                            count_stack.pop_back();
                                        }
                                        if (i != value.get_list_elements().size() - 1) {
                                            for (int j = 0; j < finished_lists - 1; j++) {
                                                std::cout << ")";
                                            }

                                            std::cout << "), ";
                                        } else {
                                            for (int j = 0; j < finished_lists; j++) {
                                                std::cout << ")";
                                            }
                                        }
                                    } else {
                                        if (i != value.get_list_elements().size() - 1)
                                            std::cout << ", ";
                                    }
                                } else {
                                    if (i != value.get_list_elements().size() - 1)
                                        std::cout << ", ";
                                }
                            }
                        }
                        std::cout << ")";
                    } else if (value.get_type_of_node() == TypeOfObject::ENVIRONMENT || value.get_value_of_node() == "dummy") {
                        std::cout << "dummy";
                    } else if (value.get_type_of_node() == TypeOfObject::LAMBDA) {
                        std::cout << "[lambda closure: ";
                        std::cout << value.get_value_of_node() << ": ";
                        std::cout << value.get_cs_index() << "]";
                    } else {
                        std::cout << value.get_value_of_node();
                    }
                }
                    else if (identifier == "Isinteger")
                    {
                        CseNode nodeValue = stack.pop_last_node_return();
                        if (nodeValue.get_type_of_node() == TypeOfObject::INTEGER)
                        {
                            stack.append_node(CseNode(TypeOfObject::BOOLEAN, "true"));
                        }
                        else
                        {
                            stack.append_node(CseNode(TypeOfObject::BOOLEAN, "false"));
                        }
                    }
                    else if (identifier == "Isstring")
                    {
                        CseNode nodeValue = stack.pop_last_node_return();
                        if (nodeValue.get_type_of_node() == TypeOfObject::STRING)
                        {
                            stack.append_node(CseNode(TypeOfObject::BOOLEAN, "true"));
                        }
                        else
                        {
                            stack.append_node(CseNode(TypeOfObject::BOOLEAN, "false"));
                        }
                    }
                    else if (identifier == "Isempty")
                    {
                        CseNode nodeValue = stack.pop_last_node_return();
                        if (nodeValue.get_type_of_node() == TypeOfObject::LIST)
                        {
                            if (nodeValue.get_list_elements().empty())
                            {
                                stack.append_node(CseNode(TypeOfObject::BOOLEAN, "true"));
                            }
                            else
                            {
                                stack.append_node(CseNode(TypeOfObject::BOOLEAN, "false"));
                            }
                        }
                        else
                        {
                            throw std::runtime_error("Invalid type for IsEmpty: " + nodeValue.get_value_of_node());
                        }
                    }
                    else if (identifier == "Istuple")
                    {
                        CseNode nodeValue = stack.pop_last_node_return();

                        if (nodeValue.get_type_of_node() == TypeOfObject::LIST)
                        {
                            stack.append_node(CseNode(TypeOfObject::BOOLEAN, "true"));
                        }
                        else
                        {
                            stack.append_node(CseNode(TypeOfObject::BOOLEAN, "false"));
                        }
                    }
                    else if (identifier == "Order")
                    {
                        CseNode nodeValue = stack.pop_last_node_return();
                        if (nodeValue.get_type_of_node() == TypeOfObject::LIST)
                        {
                            int count = 0;
                            int listSkipElement = 0;

                            for (const auto &i : nodeValue.get_list_elements())
                            {
                                if (i.get_type_of_node() == TypeOfObject::LIST && listSkipElement == 0)
                                {
                                    listSkipElement += std::stoi(i.get_value_of_node());
                                    count++;
                                }
                                else if (listSkipElement == 0)
                                {
                                    count++;
                                }
                                else
                                {
                                    listSkipElement--;
                                    continue;
                                }
                            }

                            stack.append_node(CseNode(TypeOfObject::INTEGER, std::to_string(count)));
                        }
                        else
                        {
                            throw std::runtime_error("Invalid type for Order: " + nodeValue.get_value_of_node());
                        }
                    }
                    else if (identifier == "Conc")
                    {
                        CseNode firstArg = stack.pop_last_node_return();
                        CseNode secondArg = stack.pop_last_node_return();
                        main_cs.pop_last_node();

                        if (firstArg.get_type_of_node() == TypeOfObject::STRING &&
                            (secondArg.get_type_of_node() == TypeOfObject::STRING ||
                             secondArg.get_type_of_node() == TypeOfObject::INTEGER))
                        {
                            stack.append_node(
                                CseNode(TypeOfObject::STRING, firstArg.get_value_of_node() + secondArg.get_value_of_node()));
                        }
                        else
                        {
                            throw std::runtime_error("Invalid type for Conc: " + firstArg.get_value_of_node());
                        }
                    }
                    else if (identifier == "Stem")
                    {
                        CseNode arg = stack.pop_last_node_return();

                        if (arg.get_type_of_node() == TypeOfObject::STRING)
                        {
                            stack.append_node(CseNode(TypeOfObject::STRING, arg.get_value_of_node().substr(0, 1)));
                        }
                        else
                        {
                            throw std::runtime_error("Invalid type for Stem: " + top_of_stack.get_value_of_node());
                        }
                    }
                    else if (identifier == "Stern")
                    {
                        CseNode arg = stack.pop_last_node_return();

                        if (arg.get_type_of_node() == TypeOfObject::STRING)
                        {
                            stack.append_node(CseNode(TypeOfObject::STRING, arg.get_value_of_node().substr(1)));
                        }
                        else
                        {
                            throw std::runtime_error("Invalid type for Stern: " + top_of_stack.get_value_of_node());
                        }
                    }
                    else if (identifier == "Y*")
                    {
                        CseNode lambda = stack.pop_last_node_return();

                        if (lambda.get_type_of_node() == TypeOfObject::LAMBDA)
                        {
                            if (lambda.get_is_one_bound_var())
                            {
                                stack.append_node(CseNode(TypeOfObject::EETA, lambda.get_value_of_node(), lambda.get_cs_index(),
                                                          lambda.get_environment()));
                            }
                            else
                            {
                                stack.append_node(CseNode(TypeOfObject::EETA, lambda.get_cs_index(), lambda.get_bound_variables_list(),
                                                          lambda.get_environment()));
                            }
                        }
                        else
                        {
                            throw std::runtime_error("Invalid type for Y*: " + lambda.get_value_of_node());
                        }
                    }
                    else if (identifier == "ItoS")
                    {
                        CseNode arg = stack.pop_last_node_return();

                        if (arg.get_type_of_node() == TypeOfObject::INTEGER)
                        {
                            stack.append_node(CseNode(TypeOfObject::STRING, arg.get_value_of_node()));
                        }
                        else
                        {
                            throw std::runtime_error("Invalid type for ItoS: " + arg.get_value_of_node());
                        }
                    }
                }
                else if (top_of_stack.get_type_of_node() == TypeOfObject::EETA)
                {
                    stack.append_node(top_of_stack);

                    if (top_of_stack.get_is_one_bound_var())
                    {
                        stack.append_node(
                            CseNode(TypeOfObject::LAMBDA, top_of_stack.get_value_of_node(), top_of_stack.get_cs_index(),
                                    top_of_stack.get_environment()));
                    }
                    else
                    {
                        stack.append_node(
                            CseNode(TypeOfObject::LAMBDA, top_of_stack.get_cs_index(), top_of_stack.get_bound_variables_list(),
                                    top_of_stack.get_environment()));
                    }

                    main_cs.append_node(CseNode(TypeOfObject::GAMMA, ""));
                    main_cs.append_node(CseNode(TypeOfObject::GAMMA, ""));
                }
                else if (top_of_stack.get_type_of_node() == TypeOfObject::LIST)
                {
                    CseNode secondArg = stack.pop_last_node_return();

                    if (secondArg.get_type_of_node() == TypeOfObject::INTEGER)
                    {
                        int index = std::stoi(secondArg.get_value_of_node());

                        int currentIndex = 0;
                        int listPosElement = 0;
                        int listSkipElement = 0;
                        bool isList = false;

                        for (const auto &i : top_of_stack.get_list_elements())
                        {
                            if (i.get_type_of_node() == TypeOfObject::LIST && listSkipElement == 0)
                            {
                                listSkipElement = std::stoi(i.get_value_of_node());
                                currentIndex++;

                                if (index == currentIndex)
                                {
                                    isList = true;
                                    break;
                                }
                            }
                            else if (listSkipElement == 0)
                            {
                                currentIndex++;

                                if (index == currentIndex)
                                {
                                    break;
                                }
                            }
                            else
                            {
                                listSkipElement--;
                            }
                            listPosElement++;
                        }

                        std::vector<CseNode> listOfElements = std::vector<CseNode>();

                        if (isList)
                        {
                            int length = std::stoi(top_of_stack.get_list_elements()[listPosElement].get_value_of_node());

                            for (int i = 0; i < length; i++)
                            {
                                listOfElements.push_back(top_of_stack.get_list_elements()[listPosElement + i + 1]);
                            }

                            stack.append_node(CseNode(TypeOfObject::LIST, listOfElements));
                        }
                        else
                        {
                            stack.append_node(top_of_stack.get_list_elements()[listPosElement]);
                        }
                    }
                    else
                    {
                        throw std::runtime_error("Invalid type for Index: " + secondArg.get_value_of_node());
                    }
                }

                top_of_cs = main_cs.pop_last_node_return();
            }
            else if (top_of_cs.get_type_of_node() == TypeOfObject::ENVIRONMENT)
            {
                std::vector<CseNode> environmentNodes = {};

                CseNode st_node = stack.pop_last_node_return();

                while (st_node.get_type_of_node() != TypeOfObject::ENVIRONMENT)
                {
                    environmentNodes.push_back(st_node);
                    st_node = stack.pop_last_node_return();
                }

                // push back the stack from vector
                for (auto it = environmentNodes.rbegin(); it != environmentNodes.rend(); ++it)
                {
                    stack.append_node(*it);
                }

                environment_stack.pop_back();

                top_of_cs = main_cs.pop_last_node_return();
            }
            else if (top_of_cs.get_type_of_node() == TypeOfObject::OPERATOR)
            {
                std::string operator_ = top_of_cs.get_value_of_node();

                CseNode first = stack.pop_last_node_return();
                CseNode second = stack.pop_last_node_return();

                if (operator_ == "+")
                {
                    stack.append_node(CseNode(TypeOfObject::INTEGER, std::to_string(
                                                                         std::stoi(first.get_value_of_node()) + std::stoi(second.get_value_of_node()))));
                }
                else if (operator_ == "-")
                {
                    stack.append_node(CseNode(TypeOfObject::INTEGER, std::to_string(
                                                                         std::stoi(first.get_value_of_node()) - std::stoi(second.get_value_of_node()))));
                }
                else if (operator_ == "/")
                {
                    stack.append_node(CseNode(TypeOfObject::INTEGER, std::to_string(
                                                                         std::stoi(first.get_value_of_node()) / std::stoi(second.get_value_of_node()))));
                }
                else if (operator_ == "*")
                {
                    stack.append_node(CseNode(TypeOfObject::INTEGER, std::to_string(
                                                                         std::stoi(first.get_value_of_node()) * std::stoi(second.get_value_of_node()))));
                }
                else if (operator_ == "neg")
                {
                    stack.append_node(second);
                    stack.append_node(CseNode(TypeOfObject::INTEGER, std::to_string(-std::stoi(first.get_value_of_node()))));
                }
                else if (operator_ == "not")
                {
                    stack.append_node(second);
                    if (first.get_value_of_node() == "true")
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "false"));
                    }
                    else
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "true"));
                    }
                }
                else if (operator_ == "eq")
                {
                    if (first.get_value_of_node() == second.get_value_of_node())
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "true"));
                    }
                    else
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "false"));
                    }
                }
                else if (operator_ == "gr")
                {
                    if (std::stoi(first.get_value_of_node()) > std::stoi(second.get_value_of_node()))
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "true"));
                    }
                    else
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "false"));
                    }
                }
                else if (operator_ == "ge")
                {
                    if (std::stoi(first.get_value_of_node()) >= std::stoi(second.get_value_of_node()))
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "true"));
                    }
                    else
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "false"));
                    }
                }
                else if (operator_ == "ls")
                {
                    if (std::stoi(first.get_value_of_node()) < std::stoi(second.get_value_of_node()))
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "true"));
                    }
                    else
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "false"));
                    }
                }
                else if (operator_ == "le")
                {
                    if (std::stoi(first.get_value_of_node()) <= std::stoi(second.get_value_of_node()))
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "true"));
                    }
                    else
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "false"));
                    }
                }
                else if (operator_ == "ne")
                {
                    if (first.get_value_of_node() != second.get_value_of_node())
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "true"));
                    }
                    else
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "false"));
                    }
                }
                else if (operator_ == "aug")
                {
                    if (first.get_type_of_node() == TypeOfObject::LIST)
                    {
                        if (second.get_type_of_node() == TypeOfObject::LIST)
                        {
                            std::vector<CseNode> firstElements = first.get_list_elements();
                            std::vector<CseNode> secondElements = second.get_list_elements();

                            firstElements.emplace_back(TypeOfObject::LIST, std::to_string(secondElements.size()));

                            for (auto &element : secondElements)
                            {
                                firstElements.push_back(element);
                            }

                            stack.append_node(CseNode(TypeOfObject::LIST, firstElements));
                        }
                        else if (second.get_type_of_node() == TypeOfObject::INTEGER ||
                                 second.get_type_of_node() == TypeOfObject::BOOLEAN ||
                                 second.get_type_of_node() == TypeOfObject::STRING)
                        {
                            std::vector<CseNode> firstElements = first.get_list_elements();

                            firstElements.emplace_back(second.get_type_of_node(), second.get_value_of_node());
                            stack.append_node(CseNode(TypeOfObject::LIST, firstElements));
                        }
                        else
                        {
                            throw std::runtime_error("Invalid type for aug: " + second.get_value_of_node());
                        }
                    }
                    else
                    {
                        throw std::runtime_error("Invalid type for aug: " + first.get_value_of_node());
                    }
                }
                else if (operator_ == "or")
                {
                    if (first.get_value_of_node() == "true" || second.get_value_of_node() == "true")
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "true"));
                    }
                    else
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "false"));
                    }
                }
                else if (operator_ == "&")
                {
                    if (first.get_value_of_node() == "true" && second.get_value_of_node() == "true")
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "true"));
                    }
                    else
                    {
                        stack.append_node(CseNode(TypeOfObject::BOOLEAN, "false"));
                    }
                }
                else
                {
                    throw std::runtime_error("Invalid operator: " + operator_);
                }

                top_of_cs = main_cs.pop_last_node_return();
            }
            else if (top_of_cs.get_type_of_node() == TypeOfObject::TAU)
            {
                std::vector<CseNode> tau_elements;
                int tau_size = std::stoi(top_of_cs.get_value_of_node());

                for (int i = 0; i < tau_size; i++)
                {
                    CseNode node = stack.pop_last_node_return();

                    if (node.get_type_of_node() == TypeOfObject::LIST)
                    {
                        std::vector<CseNode> firstElements = node.get_list_elements();
                        tau_elements.emplace_back(TypeOfObject::LIST, std::to_string(firstElements.size()));

                        for (auto &element : firstElements)
                        {
                            tau_elements.push_back(element);
                        }
                    }
                    else
                    {
                        tau_elements.push_back(node);
                    }
                }

                stack.append_node(CseNode(TypeOfObject::LIST, tau_elements));

                top_of_cs = main_cs.pop_last_node_return();
            }
            else if (top_of_cs.get_type_of_node() == TypeOfObject::BETA)
            {
                CseNode node = stack.pop_last_node_return();

                if (node.get_type_of_node() == TypeOfObject::BOOLEAN)
                {
                    if (node.get_value_of_node() == "true")
                    {
                        main_cs.pop_last_node();
                        CseNode true_node = main_cs.pop_last_node_return();

                        if (true_node.get_type_of_node() == TypeOfObject::DELTA)
                        {
                            main_cs.push_cs(
                                *ControlStructs[std::stoi(true_node.get_value_of_node())]);
                        }
                        else
                        {
                            throw std::runtime_error("Invalid type for beta: " + true_node.get_value_of_node());
                        }
                    }
                    else
                    {
                        CseNode false_node = main_cs.pop_last_node_return();
                        main_cs.pop_last_node();

                        if (false_node.get_type_of_node() == TypeOfObject::DELTA)
                        {
                            main_cs.push_cs(
                                *ControlStructs[std::stoi(false_node.get_value_of_node())]);
                        }
                        else
                        {
                            throw std::runtime_error("Invalid type for beta: " + false_node.get_value_of_node());
                        }
                    }
                }
                else if (node.get_type_of_node() == TypeOfObject::INTEGER)
                {
                    if (node.get_value_of_node() != "0")
                    {
                        main_cs.pop_last_node();
                        CseNode true_node = main_cs.pop_last_node_return();

                        if (true_node.get_type_of_node() == TypeOfObject::DELTA)
                        {
                            main_cs.push_cs(
                                *ControlStructs[std::stoi(true_node.get_value_of_node())]);
                        }
                        else
                        {
                            throw std::runtime_error("Invalid type for beta: " + true_node.get_value_of_node());
                        }
                    }
                    else
                    {
                        CseNode false_node = main_cs.pop_last_node_return();
                        main_cs.pop_last_node();

                        if (false_node.get_type_of_node() == TypeOfObject::DELTA)
                        {
                            main_cs.push_cs(
                                *ControlStructs[std::stoi(false_node.get_value_of_node())]);
                        }
                        else
                        {
                            throw std::runtime_error("Invalid type for beta: " + false_node.get_value_of_node());
                        }
                    }
                }
                else
                {
                    throw std::runtime_error("Invalid type for beta: " + node.get_value_of_node());
                }

                top_of_cs = main_cs.pop_last_node_return();
            }
        }
    }
};

bool is_operator(const std::string &labelOfNode)
{
    std::vector<std::string> operators_ = {"+", "-", "/", "*", "aug", "neg", "not", "eq", "gr", "ge", "ls", "le", "ne",
                                           "or", "&"};

    auto it = std::find(operators_.begin(), operators_.end(), labelOfNode);
    return it != operators_.end();
}

#endif // CSE_H

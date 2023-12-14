#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/

template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert(const std::pair<const Key, Value>& new_item) override;
    virtual void remove(const Key& key) override;

protected:
    virtual void nodeSwap(AVLNode<Key, Value>* n1, AVLNode<Key, Value>* n2);
    void rebalance(AVLNode<Key, Value>* node);
    // Helper functions for balancing
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void rotateLeftRight(AVLNode<Key, Value>* node);
    void rotateRightLeft(AVLNode<Key, Value>* node);
    // height
    int8_t getHeight(AVLNode<Key, Value>* node) const;
    void updateHeight(AVLNode<Key, Value>* node);
};

template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& new_item)
{
    AVLNode<Key, Value>* parent = nullptr;
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);

    while (current != nullptr)
    {
        parent = current;

        if (new_item.first < current->getKey())
            current = current->getLeft();
        else if (new_item.first > current->getKey())
            current = current->getRight();
        else
        {
            // Update the value for duplicate keys
            current->setValue(new_item.second);
            return;
        }
    }

    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);

    if (parent == nullptr)
    {
        // Tree is empty, make the new node the root
        this->root_ = newNode;
    }
    else if (newNode->getKey() < parent->getKey())
    {
        parent->setLeft(newNode);
    }
    else
    {
        parent->setRight(newNode);
    }

    // Update the balance and perform rebalancing
    rebalance(newNode);
}

template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if (node == nullptr)
        return;

    if (node->getLeft() != nullptr && node->getRight() != nullptr)
    {
        AVLNode<Key, Value>* predecessor = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
        nodeSwap(node, predecessor);
        node = predecessor;
    }

    AVLNode<Key, Value>* child = (node->getLeft() != nullptr) ? node->getLeft() : node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();

    if (child != nullptr)
        child->setParent(parent);

    if (parent == nullptr)
        this->root_ = child;
    else if (node == parent->getLeft())
        parent->setLeft(child);
    else
        parent->setRight(child);

    delete node;

    if (child != nullptr)
        rebalance(child);
    else if (parent != nullptr)
        rebalance(parent);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key, Value>* n1, AVLNode<Key, Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);

    int8_t tempBalance = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempBalance);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node)
{
    while (node != nullptr)
    {
        updateHeight(node);

        int8_t balance = node->getBalance();

        if (balance < -1)
        {
            // Left subtree is taller
            if (getHeight(node->getLeft()->getLeft()) >= getHeight(node->getLeft()->getRight()))
                rotateRight(node);
            else
                rotateLeftRight(node);
        }
        else if (balance > 1)
        {
            // Right subtree is taller
            if (getHeight(node->getRight()->getRight()) >= getHeight(node->getRight()->getLeft()))
                rotateLeft(node);
            else
                rotateRightLeft(node);
        }

        node = node->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* pivot = node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();

    node->setRight(pivot->getLeft());

    if (pivot->getLeft() != nullptr)
        pivot->getLeft()->setParent(node);

    pivot->setLeft(node);
    node->setParent(pivot);

    if (parent == nullptr)
        this->root_ = pivot;
    else if (node == parent->getLeft())
        parent->setLeft(pivot);
    else
        parent->setRight(pivot);

    pivot->setParent(parent);

    updateHeight(node);
    updateHeight(pivot);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* pivot = node->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();

    node->setLeft(pivot->getRight());

    if (pivot->getRight() != nullptr)
        pivot->getRight()->setParent(node);

    pivot->setRight(node);
    node->setParent(pivot);

    if (parent == nullptr)
        this->root_ = pivot;
    else if (node == parent->getLeft())
        parent->setLeft(pivot);
    else
        parent->setRight(pivot);

    pivot->setParent(parent);

    updateHeight(node);
    updateHeight(pivot);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeftRight(AVLNode<Key, Value>* node)
{
    rotateLeft(node->getLeft());
    rotateRight(node);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRightLeft(AVLNode<Key, Value>* node)
{
    rotateRight(node->getRight());
    rotateLeft(node);
}

template<class Key, class Value>
int8_t AVLTree<Key, Value>::getHeight(AVLNode<Key, Value>* node) const
{
    if (node == nullptr)
        return -1;
    return node->getBalance();
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateHeight(AVLNode<Key, Value>* node)
{
    int8_t leftHeight = getHeight(node->getLeft());
    int8_t rightHeight = getHeight(node->getRight());
    node->setBalance(std::max(leftHeight, rightHeight) + 1);
}

#endif

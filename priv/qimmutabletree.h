#ifndef QSTREE_H
#define QSTREE_H

#include "priv/qimmutabletreenode.h"

// AVL Tree Implementation
namespace QImmutable {

class Tree
{
public:
    Tree();
    ~Tree();

    bool isNull() const;

    int min() const;

    int max() const;

    int sum() const;

    int height() const;

    TreeNode *root() const;

    TreeNode* insert(int key, int count = 1);

    void remove(int key);

    TreeNode* search(int key) const;

    // Find sum of count of node wher its's key is less than input key
    int countLessThan(int key) const;

    int countLessThan(TreeNode* node) const;

    // Validate is it a balanced tree of node
    static bool validate(TreeNode* node);

private:

    void insert(TreeNode* node);

    void insert(TreeNode* current, TreeNode* node);

    void remove(TreeNode* current, int key);

    TreeNode* search(TreeNode* node, int key) const;

    TreeNode* searchMin(TreeNode* node) const;

    TreeNode* searchMax(TreeNode* node) const;

    void setRoot(TreeNode *root);

    void setSum(int sum);

    void setHeight(int height);

    void setMax(int max);

    void setMin(int min);

    void updateFromRoot();

    TreeNode* rotateLeft(TreeNode* node);

    TreeNode* rotateRight(TreeNode* node);

    int m_min;
    int m_max;
    int m_sum;
    int m_height;

    TreeNode* m_root;

};

}

QDebug operator<<(QDebug dbg, const QImmutable::Tree& change);

#endif // QSTREE_H

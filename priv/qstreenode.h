#ifndef QSTREENODE_H
#define QSTREENODE_H

namespace QImmutable {

class TreeNode
{
public:
    TreeNode();
    TreeNode(int key, int count);
    ~TreeNode();

    int count() const;
    void setCount(int count);

    int key() const;
    void setKey(int key);

    int sum() const;
    void setSum(int sum);

    int height() const;
    void setHeight(int height);

    TreeNode *left() const;
    void setLeft(TreeNode *left);

    TreeNode *right() const;
    void setRight(TreeNode *right);

    TreeNode *parent() const;
    void setParent(TreeNode *parent);

    // Remove the left node and return it. It won't be destroyed
    TreeNode* takeLeft();

    // Remove the right node and return it. It won't be destroyed
    TreeNode* takeRight();

    // If the parent is not null, remove current this node from parent.
    void unparent();

    // Update height and sum from children
    void update();

    bool hasLeft() const;

    bool hasRight() const;

    int leftHeight() const;

    int rightHeight() const;

    // Calculate the differnet of heigth from left to right child
    int balance() const;

private:

    int m_key;
    int m_count;
    int m_sum;
    int m_height;
    TreeNode* m_left;
    TreeNode* m_right;
    TreeNode* m_parent;

};

}

#endif // QSTREENODE_H

#include <QtCore>
#include "priv/qimmutabletreenode.h"

using namespace QImmutable;

TreeNode::TreeNode()
{
    m_key = 0;
    m_height = 0;
    m_count = 0;
    m_sum = 0;
    m_left = 0;
    m_right = 0;
    m_parent = 0;

}

TreeNode::TreeNode(int key, int count)
{
    m_key = key;
    m_count = count;
    m_sum = count;
    m_height = 1;
    m_left = 0;
    m_right = 0;
    m_parent = 0;
}

TreeNode::~TreeNode()
{
    if (m_left) {
        delete m_left;
    }

    if (m_right) {
        delete m_right;
    }
}

int TreeNode::count() const
{
    return m_count;
}

void TreeNode::setCount(int count)
{
    m_count = count;
}

int TreeNode::key() const
{
    return m_key;
}

void TreeNode::setKey(int key)
{
    m_key = key;
}

int TreeNode::sum() const
{
    return m_sum;
}

void TreeNode::setSum(int sum)
{
    m_sum = sum;
}

int TreeNode::height() const
{
    return m_height;
}

void TreeNode::setHeight(int height)
{
    m_height = height;
}

TreeNode *TreeNode::left() const
{
    return m_left;
}

void TreeNode::setLeft(TreeNode *left)
{
    m_left = left;
    if (left) {
        left->setParent(this);
    }
}

TreeNode *TreeNode::right() const
{
    return m_right;
}

void TreeNode::setRight(TreeNode *right)
{
    m_right = right;
    if (right) {
        right->setParent(this);
    }
}

void TreeNode::update()
{
    int h = 1;
    m_sum = m_count;
    if (m_left) {
        h = qMax(h,m_left->height() + 1);
        m_sum += m_left->sum();
    }

    if (m_right) {
        h = qMax(h, m_right->height() + 1);
        m_sum += m_right->sum();
    }

    m_height = h;
}

bool TreeNode::hasLeft() const
{
    return m_left != 0;
}

bool TreeNode::hasRight() const
{
    return m_right != 0;
}

int TreeNode::leftHeight() const
{
    int ret = 0;
    if (m_left) {
        ret = m_left->height();
    }
    return ret;
}

int TreeNode::rightHeight() const
{
    int ret = 0;
    if (m_right) {
        ret = m_right->height();
    }
    return ret;
}

int TreeNode::balance() const
{
    int l = 0, r = 0;
    if (m_left) {
        l = m_left->height();
    }

    if (m_right) {
        r = m_right->height();
    }

    return l - r;
}

TreeNode *TreeNode::parent() const
{
    return m_parent;
}

void TreeNode::setParent(TreeNode *parent)
{
    m_parent = parent;
}

TreeNode *TreeNode::takeLeft()
{
    TreeNode* res = 0;

    if (m_left) {
        res = m_left;
        m_left->setParent(0);
        m_left = 0;
    }

    return res;
}

TreeNode *TreeNode::takeRight()
{
    TreeNode* res = 0;

    if (m_right) {
        res = m_right;
        m_right->setParent(0);
        m_right = 0;
    }

    return res;
}

void TreeNode::unparent()
{
    if (m_parent) {
        if (m_parent->left() == this) {
            m_parent->takeLeft();
        } else if (m_parent->right() == this) {
            m_parent->takeRight();
        } else {
            qWarning() << "QSTreeNode::unparent(): Invalid tree structure";
        }
    }
}


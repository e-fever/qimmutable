#include <QtCore>
#include "priv/qimmutabletree.h"

using namespace QImmutable;

Tree::Tree()
{
    m_min = m_max = m_sum = m_height = 0;
    m_root = 0;
}

Tree::~Tree()
{
    if (m_root) {
        delete m_root;
    }
}

bool Tree::isNull() const
{
    return m_root == 0;
}

int Tree::min() const
{
    return m_min;
}

void Tree::setMin(int min)
{
    m_min = min;
}

void Tree::updateFromRoot()
{
    if (m_root) {
        m_sum = m_root->sum();
        m_height = m_root->height();
    } else {
        m_sum = 0;
        m_height = 0;
    }
}

TreeNode* Tree::rotateLeft(TreeNode *n)
{
    /* n            x
     *  \         /
     *   x       n
     *  /         \
     * y            y
     */

    bool isLeft = false;
    TreeNode* parent = n->parent();

    if (parent) {
        isLeft = (parent->left() == n);
    }

    TreeNode* x = n->takeRight();
    TreeNode* y = x->takeLeft();

    x->setLeft(n);
    n->setRight(y);

    n->update();
    x->update();

    if (parent) {
        if (isLeft) {
            parent->setLeft(x);
        } else {
            parent->setRight(x);
        }
    } else {
        // It is root
        m_root = x;
        updateFromRoot();
    }

    return x;
}

TreeNode *Tree::rotateRight(TreeNode *n)
{
    /*   n      x
     *  /         \
     * x           n
     *  \         /
     *   y       y
     */

    bool isLeft = false;
    TreeNode* parent = n->parent();

    if (parent) {
        isLeft = (parent->left() == n);
    }

    TreeNode* x = n->takeLeft();
    TreeNode* y = x->takeRight();

    x->setRight(n);
    n->setLeft(y);

    n->update();
    x->update();

    if (parent) {
        if (isLeft) {
            parent->setLeft(x);
        } else {
            parent->setRight(x);
        }
    } else {
        // It is root
        m_root = x;
        updateFromRoot();
    }

    return x;
}

int Tree::max() const
{
    return m_max;
}

void Tree::setMax(int max)
{
    m_max = max;
}

int Tree::sum() const
{
    return m_sum;
}

void Tree::setSum(int sum)
{
    m_sum = sum;
}

int Tree::height() const
{
    return m_height;
}

void Tree::setHeight(int depth)
{
    m_height = depth;
}

TreeNode *Tree::root() const
{
    return m_root;
}

void Tree::setRoot(TreeNode *root)
{
    m_root = root;
    if (root) {
        m_min = root->key();
        m_max = root->key();
        m_sum = root->count();
    } else {
        m_min = 0;
        m_max = 0;
        m_sum = 0;
    }
}

TreeNode *Tree::insert(int key, int count)
{
    TreeNode* node = new TreeNode(key, count);
    insert(node);
    return node;
}

void Tree::remove(int key)
{
    if (m_root == 0) {
        return;
    }

    remove(m_root, key);
    updateFromRoot();

    if (key == m_min && m_root) {
        TreeNode* node = searchMin(m_root);
        m_min = node->key();
    }

    // Remarks: It don't update max. Becoz it is not needed in Diff algorithm.

}

TreeNode *Tree::search(int key) const
{
    if (m_root == 0) {
        return 0;
    }

    return search(m_root, key);
}

int Tree::countLessThan(int key) const
{
    TreeNode* node = search(key);

    if (node == 0) {
        return 0;
    }

    return countLessThan(node);
}

int Tree::countLessThan(TreeNode *node) const
{
    int sum = 0;
    bool fromRightChild = false;
    int key = node->key();

    while (node != 0) {

        if (!fromRightChild && node->left()) {
            sum += node->left()->sum();
        }

        if (node->key() < key) {
            sum += node->count();
        }

        if (node->parent()) {
            fromRightChild = node->parent()->right() != node;
        }

        node = node->parent();
    }

    return sum;
}

bool Tree::validate(TreeNode* node)
{

    if (node == 0) {
        return false;
    }

    bool invalid;

    int balance = qAbs(node->balance());

    invalid = (balance >= 2);

    if (invalid) {
        qWarning() << QString("Node[%1] is not balanced").arg(node->key());
    } else {

        if (node->hasLeft()) {
            invalid |= !validate(node->left());
        }

        if (node->hasRight()) {
            invalid |= !validate((node->right()));
        }

    }

    return !invalid;
}

void Tree::insert(TreeNode *node)
{
    if (m_root == 0) {
        setRoot(node);
    } else {
        insert(m_root, node);

        if (m_min > node->key()) {
            m_min = node->key();
        }
        if (m_max < node->key()) {
            m_max = node->key();
        }
    }

    updateFromRoot();
}

void Tree::insert(TreeNode *current, TreeNode *node)
{
    if (node->key() < current->key()) {
        if (current->hasLeft()) {
            insert(current->left(), node);
        } else {
            current->setLeft(node);
        }
    } else {
        if (current->hasRight()) {
            insert(current->right(), node);
        } else {
            current->setRight(node);
        }
    }

    int balance = current->leftHeight() - current->rightHeight();

    if (balance > 1 && node->key() < current->left()->key()) {
        // Left Left Case
        rotateRight(current);
    } else if (balance < -1 && node->key() > current->right()->key()) {
        // Right Right Case
        rotateLeft(current);
    } else if (balance > 1 && node->key() > current->left()->key()) {
        // Left Right Case
        rotateLeft(current->left());
        rotateRight(current);
    } else if (balance < -1 && node->key() < current->right()->key()) {
        rotateRight(current->right());
        rotateLeft(current);
    }

    current->update();
}

void Tree::remove(TreeNode *node, int key)
{
    if (node->key() != key) {

        if (key < node->key()) {
            if (node->hasLeft()) {
                remove(node->left(), key);
            }
        } else {
            if (node->hasRight()) {
                remove(node->right(), key);
            }
        }

    } else {

        if (node->hasLeft() && node->hasRight()) {
            TreeNode* minNode = searchMin(node->right());
            node->setCount(minNode->count());
            node->setKey(minNode->key());
            remove(node->right(), minNode->key());

        } else {

            TreeNode* parent = node->parent();
            TreeNode* child = 0;
            if (node->hasLeft()) {
                child = node->takeLeft();
            } else if (node->hasRight()) {
                child = node->takeRight();
            }

            if (parent) {
                if (parent->left() == node) {
                    parent->setLeft(child);
                } else {
                    parent->setRight(child);
                }
            } else {
                m_root = child;
            }

            delete node;
            node = child; // Replace current node
        }
    }

    if (node) {
        // The node is not deleted or replaced by its child

        int balance = node->balance();

        if (balance > 1 && node->left()->balance() >= 0) {
            rotateRight(node);
        } else if (balance > 1 && node->left()->balance() < 0) {
            rotateLeft(node->left());
            rotateRight(node);
        } else if (balance < -1 && node->right()->balance() <= 0) {
            rotateLeft(node);
        } else if (balance < -1 && node->right()->balance() > 0) {
            rotateRight(node->right());
            rotateLeft(node);
        }

        node->update();
    }
}

TreeNode* Tree::search(TreeNode *node, int key) const
{
    TreeNode* res = 0;

    if (node->key() == key) {
        return node;
    } else if (key < node->key() ) {
        if (node->left() != 0) {
            res = search(node->left(), key);
        }
    } else {
        if (node->right() != 0) {
            res = search(node->right(), key);
        }
    }
    return res;
}

TreeNode *Tree::searchMin(TreeNode *node) const
{
    if (!node->hasLeft()) {
        return node;
    } else {
        return searchMin(node->left());
    }
}

TreeNode *Tree::searchMax(TreeNode *node) const
{
    if (!node->hasRight()) {
        return node;
    } else {
        return searchMax(node->right());
    }
}

QDebug operator<<(QDebug dbg, const Tree& tree) {

    QQueue<TreeNode*> queue;
    QStringList links;

    int height = -1;

    if (tree.root() != 0) {
        queue.enqueue(tree.root());
        links.append(QString("[%1]").arg(tree.root()->key()));
        height = tree.root()->height();
    }

    while (queue.size() > 0) {
        TreeNode* node = queue.dequeue();

        if (node->parent() != 0) {
            links << QString("%1->%2").arg(node->parent()->key()).arg(node->key());
        }

        if (node->hasLeft()) {
            queue.enqueue(node->left());
        }

        if (node->hasRight()) {
            queue.enqueue(node->right());
        }

    }

    dbg << links.join(" , ");
    return dbg;
}


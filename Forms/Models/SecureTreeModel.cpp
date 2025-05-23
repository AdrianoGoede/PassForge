#include "SecureTreeModel.h"

SecureTreeModel::SecureTreeModel(QObject* parent) : QAbstractItemModel(parent) { this->rootNode = std::make_unique<SecureTreeNode>("Root"); }

QModelIndex SecureTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (column < 0 || row < 0) return QModelIndex();

    SecureTreeNode* parentNode = this->nodeFromIndex(parent);
    if (row >= static_cast<int>(parentNode->children.size())) return QModelIndex();

    SecureTreeNode* childNode = parentNode->children[row].get();
    return createIndex(row, column, childNode);
}

QModelIndex SecureTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) return QModelIndex();

    SecureTreeNode* childNode = this->nodeFromIndex(index);
    SecureTreeNode* parentNode = (childNode ? childNode->parent : nullptr);

    if (!parentNode || parentNode == this->rootNode.get()) return QModelIndex();

    return createIndex(parentNode->row(), 0, parentNode);
}

int SecureTreeModel::rowCount(const QModelIndex& parent) const { return static_cast<int>(this->nodeFromIndex(parent)->children.size()); }

int SecureTreeModel::columnCount(const QModelIndex& parent) const { return 1; }

QVariant SecureTreeModel::data(const QModelIndex& index, int role) const { return (!index.isValid() || role != Qt::DisplayRole) ? QVariant() : this->nodeFromIndex(index)->value; }

void SecureTreeModel::addItem(const QModelIndex& parent, const SecureQByteArray& value)
{
    SecureTreeNode* parentNode = this->nodeFromIndex(parent);
    int position = static_cast<int>(parentNode->children.size());

    beginInsertRows(parent, position, position);
    std::unique_ptr<SecureTreeNode> child = std::make_unique<SecureTreeNode>(value, parentNode);
    parentNode->children.push_back(std::move(child));
    endInsertRows();
}

SecureTreeNode* SecureTreeModel::nodeFromIndex(const QModelIndex& index) const { return index.isValid() ? static_cast<SecureTreeNode*>(index.internalPointer()) : this->rootNode.get(); }

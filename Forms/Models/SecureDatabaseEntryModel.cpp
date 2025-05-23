#include "SecureDatabaseEntryModel.h"
#include <QString>

SecureDatabaseEntryListModel::SecureDatabaseEntryListModel(QObject *parent) : QAbstractItemModel(parent) { this->rootNode = std::make_unique<SecureDatabaseEntryListNode>(DatabaseEntry()); }

QModelIndex SecureDatabaseEntryListModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column < 0 || row < 0) return QModelIndex();

    SecureDatabaseEntryListNode* parentNode = this->nodeFromIndex(parent);
    if (row >= static_cast<int>(parentNode->children.size())) return QModelIndex();

    SecureDatabaseEntryListNode* childNode = parentNode->children[row].get();
    return createIndex(row, column, childNode);
}

QModelIndex SecureDatabaseEntryListModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) return QModelIndex();

    SecureDatabaseEntryListNode* childNode = this->nodeFromIndex(index);
    SecureDatabaseEntryListNode* parentNode = (childNode ? childNode->parent : nullptr);

    if (!parentNode || parentNode == this->rootNode.get()) return QModelIndex();

    return createIndex(parentNode->row(), 0, parentNode);
}

int SecureDatabaseEntryListModel::rowCount(const QModelIndex &parent) const { return static_cast<int>(this->nodeFromIndex(parent)->children.size()); }

int SecureDatabaseEntryListModel::columnCount(const QModelIndex &parent) const { return 1; }

QVariant SecureDatabaseEntryListModel::data(const QModelIndex& index, int role) const
{
    DatabaseEntry entry = this->nodeFromIndex(index)->value;
    return ((!index.isValid() || role != Qt::DisplayRole) ? QVariant() : SecureQByteArray(QString("%1 - %2").arg(entry.getEntryId()).arg(entry.getName().data()).toUtf8()));
}

const DatabaseEntry& SecureDatabaseEntryListModel::getDbEntry() const { return this->rootNode->value; }

void SecureDatabaseEntryListModel::addItem(const QModelIndex& parent, const DatabaseEntry& value)
{
    SecureDatabaseEntryListNode* parentNode = this->nodeFromIndex(parent);
    int position = static_cast<int>(parentNode->children.size());

    beginInsertRows(parent, position, position);
    std::unique_ptr<SecureDatabaseEntryListNode> child = std::make_unique<SecureDatabaseEntryListNode>(value, parentNode);
    parentNode->children.push_back(std::move(child));
    endInsertRows();
}

SecureDatabaseEntryListNode *SecureDatabaseEntryListModel::nodeFromIndex(const QModelIndex& index) const { return index.isValid() ? static_cast<SecureDatabaseEntryListNode*>(index.internalPointer()) : this->rootNode.get(); }

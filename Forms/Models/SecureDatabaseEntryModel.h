#ifndef SECUREDATABASEENTRYMODEL_H
#define SECUREDATABASEENTRYMODEL_H

#include "../../Database/DatabaseEntry.h"
#include <QAbstractItemModel>
#include <QObject>

struct SecureDatabaseEntryListNode {
    DatabaseEntry value;
    SecureDatabaseEntryListNode* parent = nullptr;
    std::vector<std::unique_ptr<SecureDatabaseEntryListNode>> children;

    explicit SecureDatabaseEntryListNode(const DatabaseEntry& val, SecureDatabaseEntryListNode* parentNode = nullptr) : value(val), parent(parentNode) {}

    void secureClear() {
        for (auto& child : children)
            child->secureClear();
        children.clear();
    }

    int row() const {
        if (!parent) return 0;
        for (size_t i = 0; i < parent->children.size(); ++i)
            if (parent->children[i].get() == this)
                return static_cast<int>(i);
        return 0;
    }
};

class SecureDatabaseEntryListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit SecureDatabaseEntryListModel(QObject *parent = nullptr);
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    DatabaseEntry& getDbEntry(const QModelIndex& index) const;
    void addItem(const QModelIndex& parent, const DatabaseEntry& value);

private:
    std::unique_ptr<SecureDatabaseEntryListNode> rootNode;
    SecureDatabaseEntryListNode* nodeFromIndex(const QModelIndex& index) const;
};

#endif // SECUREDATABASEENTRYMODEL_H

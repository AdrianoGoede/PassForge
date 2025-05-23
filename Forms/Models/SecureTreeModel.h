#ifndef SECURETREEMODEL_H
#define SECURETREEMODEL_H

#include "../../Crypto/SecureQByteArray.h"
#include <QAbstractItemModel>
#include <QObject>

struct SecureTreeNode {
    SecureQByteArray value;
    SecureTreeNode* parent = nullptr;
    std::vector<std::unique_ptr<SecureTreeNode>> children;

    explicit SecureTreeNode(const SecureQByteArray& val, SecureTreeNode* parentNode = nullptr) : value(val), parent(parentNode) {}

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

class SecureTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit SecureTreeModel(QObject *parent = nullptr);
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    void addItem(const QModelIndex& parent, const SecureQByteArray& value);

private:
    std::unique_ptr<SecureTreeNode> rootNode;
    SecureTreeNode* nodeFromIndex(const QModelIndex& index) const;
};

#endif // SECURETREEMODEL_H

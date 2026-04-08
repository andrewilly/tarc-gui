#pragma once

#include <QAbstractItemModel>
#include <QIcon>
#include <vector>
#include <string>

struct ArchiveEntry {
    std::string name;
    uint64_t origSize;
    uint64_t compSize;
    uint8_t codec;
    bool isDirectory;
    int parent;
    std::vector<int> children;
};

class ArchiveModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit ArchiveModel(QObject* parent = nullptr);
    ~ArchiveModel();
    
    bool loadArchive(const QString& path);
    void clear();
    
    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    
    // Utility
    QString formatSize(uint64_t bytes) const;
    QString formatRatio(uint64_t orig, uint64_t comp) const;

private:
    void buildTree();
    QIcon iconForFile(const QString& name) const;
    
    std::vector<ArchiveEntry> m_entries;
    std::vector<int> m_rootItems;
    QString m_archivePath;
};
#include "archivemodel.h"
#include <core/tarc_wrapper.h>
#include <QFileInfo>
#include <QDir>

ArchiveModel::ArchiveModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

ArchiveModel::~ArchiveModel()
{
}

bool ArchiveModel::loadArchive(const QString& path)
{
    clear();
    m_archivePath = path;
    
    // TODO: Implementare caricamento effettivo dell'archivio
    // Usare TarcWrapper per leggere il TOC
    
    return true;
}

void ArchiveModel::clear()
{
    beginResetModel();
    m_entries.clear();
    m_rootItems.clear();
    endResetModel();
}

void ArchiveModel::buildTree()
{
    // TODO: Costruire struttura ad albero dai file
}

QModelIndex ArchiveModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    
    int parentId = parent.isValid() ? parent.internalId() : -1;
    
    // Trova l'entry con il parent corretto
    for (size_t i = 0; i < m_entries.size(); ++i) {
        if (m_entries[i].parent == parentId) {
            if (row == 0) {
                return createIndex(row, column, static_cast<int>(i));
            }
            row--;
        }
    }
    
    return QModelIndex();
}

QModelIndex ArchiveModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();
    
    int entryId = child.internalId();
    if (entryId < 0 || entryId >= static_cast<int>(m_entries.size()))
        return QModelIndex();
    
    int parentId = m_entries[entryId].parent;
    if (parentId == -1)
        return QModelIndex();
    
    // Trova il row del parent
    int row = 0;
    for (size_t i = 0; i < m_entries.size(); ++i) {
        if (m_entries[i].parent == m_entries[parentId].parent) {
            if (static_cast<int>(i) == parentId)
                return createIndex(row, 0, parentId);
            row++;
        }
    }
    
    return QModelIndex();
}

int ArchiveModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return static_cast<int>(m_rootItems.size());
    
    int parentId = parent.internalId();
    if (parentId < 0 || parentId >= static_cast<int>(m_entries.size()))
        return 0;
    
    return static_cast<int>(m_entries[parentId].children.size());
}

int ArchiveModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 5; // Nome, Codec, Originale, Compresso, Ratio
}

QVariant ArchiveModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    int entryId = index.internalId();
    if (entryId < 0 || entryId >= static_cast<int>(m_entries.size()))
        return QVariant();
    
    const auto& entry = m_entries[entryId];
    
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return QString::fromStdString(entry.name);
            case 1: {
                switch (entry.codec) {
                    case 0: return "ZSTD";
                    case 1: return "LZ4";
                    case 2: return "LZMA";
                    case 3: return "NONE";
                    default: return "???";
                }
            }
            case 2: return formatSize(entry.origSize);
            case 3: return formatSize(entry.compSize);
            case 4: return formatRatio(entry.origSize, entry.compSize);
            default: return QVariant();
        }
    }
    
    if (role == Qt::DecorationRole && index.column() == 0) {
        return iconForFile(QString::fromStdString(entry.name));
    }
    
    return QVariant();
}

QVariant ArchiveModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return "Nome file";
            case 1: return "Codec";
            case 2: return "Dimensione";
            case 3: return "Compresso";
            case 4: return "Ratio";
            default: return QVariant();
        }
    }
    return QVariant();
}

QString ArchiveModel::formatSize(uint64_t bytes) const
{
    if (bytes < 1024)
        return QString("%1 B").arg(bytes);
    if (bytes < 1024 * 1024)
        return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 1);
    if (bytes < 1024 * 1024 * 1024)
        return QString("%1 MB").arg(bytes / (1024.0 * 1024.0), 0, 'f', 1);
    return QString("%1 GB").arg(bytes / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
}

QString ArchiveModel::formatRatio(uint64_t orig, uint64_t comp) const
{
    if (orig == 0) return "0%";
    double ratio = 100.0 * (1.0 - static_cast<double>(comp) / orig);
    return QString("%1%").arg(ratio, 0, 'f', 1);
}

QIcon ArchiveModel::iconForFile(const QString& name) const
{
    QFileInfo info(name);
    QString ext = info.suffix().toLower();
    
    if (info.isDir() || name.endsWith('/'))
        return QIcon::fromTheme("folder", QIcon(":/icons/folder.png"));
    
    if (ext == "txt" || ext == "md" || ext == "cpp" || ext == "h" || ext == "py")
        return QIcon::fromTheme("text-x-generic", QIcon(":/icons/file-text.png"));
    
    if (ext == "jpg" || ext == "png" || ext == "gif" || ext == "svg")
        return QIcon::fromTheme("image-x-generic", QIcon(":/icons/image.png"));
    
    return QIcon::fromTheme("text-x-generic", QIcon(":/icons/file.png"));
}
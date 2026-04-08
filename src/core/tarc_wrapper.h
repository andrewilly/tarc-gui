#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

class TarcWrapper : public QObject
{
    Q_OBJECT

public:
    explicit TarcWrapper(QObject *parent = nullptr);
    
    bool compress(const QString& archive, const QStringList& files, bool append, int level, QString& error);
    bool extract(const QString& archive, const QString& targetDir, QString& error);
    bool remove(const QString& archive, const QStringList& files, QString& error);
    bool test(const QString& archive, QString& error);

signals:
    void progressUpdated(int percent, const QString& currentFile);
};

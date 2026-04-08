#pragma once

#include <QThread>
#include <QString>
#include <QStringList>

class WorkerThread : public QThread
{
    Q_OBJECT

public:
    enum Operation {
        OpCompress,
        OpExtract,
        OpDelete,
        OpTest
    };
    
    explicit WorkerThread(QObject* parent = nullptr);
    ~WorkerThread();
    
    void runCompress(const QString& archive, 
                     const QStringList& files,
                     int level,
                     bool append);
                     
    void runExtract(const QString& archive, const QString& targetDir);
    void runDelete(const QString& archive, const QStringList& files);
    void runTest(const QString& archive);

signals:
    void progressUpdated(int percent, const QString& currentFile);
    void operationFinished(bool success, const QString& message);
    void logMessage(const QString& message, int type);

protected:
    void run() override;

private:
    Operation m_operation;
    QString m_archive;
    QString m_targetDir;
    QStringList m_files;
    int m_level;
    bool m_append;
};
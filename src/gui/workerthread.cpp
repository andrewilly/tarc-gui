#include "workerthread.h"
#include <core/tarc_wrapper.h>

WorkerThread::WorkerThread(QObject* parent)
    : QThread(parent)
    , m_level(3)
    , m_append(false)
{
}

WorkerThread::~WorkerThread()
{
    if (isRunning()) {
        quit();
        wait();
    }
}

void WorkerThread::runCompress(const QString& archive, 
                                const QStringList& files,
                                int level,
                                bool append)
{
    m_operation = OpCompress;
    m_archive = archive;
    m_files = files;
    m_level = level;
    m_append = append;
    start();
}

void WorkerThread::runExtract(const QString& archive, const QString& targetDir)
{
    m_operation = OpExtract;
    m_archive = archive;
    m_targetDir = targetDir;
    start();
}

void WorkerThread::runDelete(const QString& archive, const QStringList& files)
{
    m_operation = OpDelete;
    m_archive = archive;
    m_files = files;
    start();
}

void WorkerThread::runTest(const QString& archive)
{
    m_operation = OpTest;
    m_archive = archive;
    start();
}

void WorkerThread::run()
{
    TarcWrapper wrapper;
    
    // Connect progress signals
    connect(&wrapper, &TarcWrapper::progressUpdated,
            this, &WorkerThread::progressUpdated);
    
    bool success = false;
    QString message;
    
    switch (m_operation) {
        case OpCompress:
            success = wrapper.compress(m_archive, m_files, m_append, m_level, message);
            break;
        case OpExtract:
            success = wrapper.extract(m_archive, m_targetDir, message);
            break;
        case OpDelete:
            success = wrapper.remove(m_archive, m_files, message);
            break;
        case OpTest:
            success = wrapper.test(m_archive, message);
            break;
    }
    
    emit operationFinished(success, message);
}
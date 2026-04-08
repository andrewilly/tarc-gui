#include "tarc_wrapper.h"
#include <QDebug>

// Questi includono sono per il core TARC
#include "engine.h"
#include "io.h"
#include "license.h"

TarcWrapper::TarcWrapper(QObject *parent)
    : QObject(parent)
{
}

bool TarcWrapper::compress(const QString& archive, const QStringList& files, bool append, int level, QString& error)
{
    Q_UNUSED(archive)
    Q_UNUSED(files)
    Q_UNUSED(append)
    Q_UNUSED(level)
    Q_UNUSED(error)
    
    // TODO: Implementare chiamata reale al core TARC
    // Per ora simuliamo progresso
    for (int i = 0; i <= 100; i += 10) {
        emit progressUpdated(i, QString("Compressing... %1%").arg(i));
        // Simula lavoro
        QThread::msleep(100);
    }
    
    return true;
}

bool TarcWrapper::extract(const QString& archive, const QString& targetDir, QString& error)
{
    Q_UNUSED(archive)
    Q_UNUSED(targetDir)
    Q_UNUSED(error)
    
    // TODO: Implementare chiamata reale al core TARC
    for (int i = 0; i <= 100; i += 10) {
        emit progressUpdated(i, QString("Extracting... %1%").arg(i));
        QThread::msleep(100);
    }
    
    return true;
}

bool TarcWrapper::remove(const QString& archive, const QStringList& files, QString& error)
{
    Q_UNUSED(archive)
    Q_UNUSED(files)
    Q_UNUSED(error)
    
    // TODO: Implementare chiamata reale al core TARC
    emit progressUpdated(100, "Removing files...");
    
    return true;
}

bool TarcWrapper::test(const QString& archive, QString& error)
{
    Q_UNUSED(archive)
    Q_UNUSED(error)
    
    // TODO: Implementare chiamata reale al core TARC
    for (int i = 0; i <= 100; i += 20) {
        emit progressUpdated(i, QString("Testing... %1%").arg(i));
        QThread::msleep(100);
    }
    
    return true;
}

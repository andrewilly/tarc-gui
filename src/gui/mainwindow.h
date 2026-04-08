#pragma once

#include <QMainWindow>
#include <QTreeWidget>
#include <QProgressBar>
#include <QLabel>
#include <memory>

QT_BEGIN_NAMESPACE
class QAction;
class QToolBar;
class QStatusBar;
QT_END_NAMESPACE

class ArchiveModel;
class WorkerThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onOpenArchive();
    void onAddFiles();
    void onExtract();
    void onDeleteFiles();
    void onTestArchive();
    void onSettings();
    void onAbout();
    
    void onOperationProgress(int percent, const QString& currentFile);
    void onOperationFinished(bool success, const QString& message);
    void onArchiveLoaded(const QString& path);

private:
    void setupUI();
    void createMenuBar();
    void createToolBar();
    void createStatusBar();
    void updateArchiveList(const QString& path);
    void setBusy(bool busy);
    void updateStatus(const QString& message);
    
    // UI Components
    QTreeWidget* m_archiveTree;
    QProgressBar* m_progressBar;
    QLabel* m_statusLabel;
    QLabel* m_sizeLabel;
    QLabel* m_fileCountLabel;
    
    // Actions
    QAction* m_openAction;
    QAction* m_addAction;
    QAction* m_extractAction;
    QAction* m_deleteAction;
    QAction* m_testAction;
    QAction* m_settingsAction;
    
    // Core
    std::unique_ptr<ArchiveModel> m_model;
    WorkerThread* m_worker;
    
    QString m_currentArchive;
};
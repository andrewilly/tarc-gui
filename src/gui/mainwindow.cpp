#include "mainwindow.h"
#include "archivemodel.h"
#include "workerthread.h"
#include "settingsdialog.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QTreeWidget>
#include <QProgressBar>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_model(std::make_unique<ArchiveModel>())
    , m_worker(nullptr)
{
    setupUI();
    createMenuBar();
    createToolBar();
    createStatusBar();
    
    setWindowTitle("TARC Archiver");
    resize(900, 600);
}

MainWindow::~MainWindow()
{
    if (m_worker && m_worker->isRunning()) {
        m_worker->quit();
        m_worker->wait();
    }
}

void MainWindow::setupUI()
{
    // Tree widget per l'archivio
    m_archiveTree = new QTreeWidget(this);
    m_archiveTree->setHeaderLabels(QStringList() << "Nome" << "Codec" << "Originale" << "Compresso" << "Ratio");
    m_archiveTree->setAlternatingRowColors(true);
    m_archiveTree->setIndentation(20);
    m_archiveTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    
    // Imposta larghezze colonne
    m_archiveTree->header()->setStretchLastSection(false);
    m_archiveTree->setColumnWidth(0, 350);
    m_archiveTree->setColumnWidth(1, 60);
    m_archiveTree->setColumnWidth(2, 100);
    m_archiveTree->setColumnWidth(3, 100);
    m_archiveTree->setColumnWidth(4, 80);
    
    setCentralWidget(m_archiveTree);
    
    // Progress bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    
    // Status labels
    m_statusLabel = new QLabel("Pronto", this);
    m_sizeLabel = new QLabel("", this);
    m_fileCountLabel = new QLabel("", this);
}

void MainWindow::createMenuBar()
{
    auto* fileMenu = menuBar()->addMenu("&File");
    
    m_openAction = fileMenu->addAction("&Apri archivio...", this, &MainWindow::onOpenArchive);
    m_openAction->setShortcut(QKeySequence::Open);
    
    fileMenu->addSeparator();
    
    m_addAction = fileMenu->addAction("&Aggiungi file...", this, &MainWindow::onAddFiles);
    m_extractAction = fileMenu->addAction("&Estrai...", this, &MainWindow::onExtract);
    m_deleteAction = fileMenu->addAction("&Elimina...", this, &MainWindow::onDeleteFiles);
    
    fileMenu->addSeparator();
    
    m_testAction = fileMenu->addAction("&Test integrità...", this, &MainWindow::onTestArchive);
    
    fileMenu->addSeparator();
    
    fileMenu->addAction("E&sci", this, &QWidget::close, QKeySequence::Quit);
    
    auto* helpMenu = menuBar()->addMenu("&?");
    helpMenu->addAction("&Informazioni su...", this, &MainWindow::onAbout);
    helpMenu->addAction("&Impostazioni...", this, &MainWindow::onSettings);
}

void MainWindow::createToolBar()
{
    auto* toolbar = addToolBar("Main");
    toolbar->setMovable(false);
    
    toolbar->addAction(m_openAction);
    toolbar->addSeparator();
    toolbar->addAction(m_addAction);
    toolbar->addAction(m_extractAction);
    toolbar->addAction(m_deleteAction);
    toolbar->addAction(m_testAction);
    toolbar->addSeparator();
    toolbar->addAction(m_settingsAction);
}

void MainWindow::createStatusBar()
{
    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addWidget(m_fileCountLabel);
    statusBar()->addWidget(m_sizeLabel);
    statusBar()->addWidget(m_progressBar);
}

void MainWindow::onOpenArchive()
{
    QString filename = QFileDialog::getOpenFileName(this, 
        "Apri archivio TARC", 
        QString(),
        "TARC Archives (*.tar4);;All Files (*)");
    
    if (filename.isEmpty()) return;
    
    updateArchiveList(filename);
}

void MainWindow::onAddFiles()
{
    if (m_currentArchive.isEmpty()) {
        onOpenArchive();
        if (m_currentArchive.isEmpty()) return;
    }
    
    QStringList files = QFileDialog::getOpenFileNames(this,
        "Seleziona file da aggiungere",
        QString(),
        "All Files (*)");
    
    if (files.isEmpty()) return;
    
    // Crea dialog compressione
    QDialog dialog(this);
    dialog.setWindowTitle("Compressione");
    dialog.resize(500, 400);
    
    // ... implementazione dialog compressione
}

void MainWindow::onExtract()
{
    if (m_currentArchive.isEmpty()) {
        onOpenArchive();
        if (m_currentArchive.isEmpty()) return;
    }
    
    QString targetDir = QFileDialog::getExistingDirectory(this,
        "Seleziona cartella di destinazione");
    
    if (targetDir.isEmpty()) return;
    
    setBusy(true);
    updateStatus("Estrazione in corso...");
    
    m_worker = new WorkerThread(this);
    connect(m_worker, &WorkerThread::progressUpdated,
            this, &MainWindow::onOperationProgress);
    connect(m_worker, &WorkerThread::operationFinished,
            this, &MainWindow::onOperationFinished);
    
    m_worker->runExtract(m_currentArchive, targetDir);
}

void MainWindow::onDeleteFiles()
{
    // Implementazione
}

void MainWindow::onTestArchive()
{
    if (m_currentArchive.isEmpty()) {
        onOpenArchive();
        if (m_currentArchive.isEmpty()) return;
    }
    
    setBusy(true);
    updateStatus("Test in corso...");
    
    m_worker = new WorkerThread(this);
    connect(m_worker, &WorkerThread::progressUpdated,
            this, &MainWindow::onOperationProgress);
    connect(m_worker, &WorkerThread::operationFinished,
            this, &MainWindow::onOperationFinished);
    
    m_worker->runTest(m_currentArchive);
}

void MainWindow::onSettings()
{
    SettingsDialog dialog(this);
    dialog.exec();
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "Informazioni su TARC",
        "<h2>TARC Archiver</h2>"
        "<p>Versione: 1.0.0</p>"
        "<p>Archiver ibrido con compressione automatica</p>"
        "<p>Codec supportati: ZSTD, LZ4, LZMA</p>"
        "<p><a href='https://github.com/tuoaccount/tarc'>GitHub</a></p>");
}

void MainWindow::onOperationProgress(int percent, const QString& currentFile)
{
    m_progressBar->setVisible(true);
    m_progressBar->setValue(percent);
    updateStatus(QString("Processando: %1").arg(currentFile));
}

void MainWindow::onOperationFinished(bool success, const QString& message)
{
    m_progressBar->setVisible(false);
    setBusy(false);
    
    if (success) {
        updateStatus(message);
        // Ricarica l'archivio
        if (!m_currentArchive.isEmpty()) {
            updateArchiveList(m_currentArchive);
        }
    } else {
        QMessageBox::critical(this, "Errore", message);
        updateStatus("Operazione fallita: " + message);
    }
}

void MainWindow::onArchiveLoaded(const QString& path)
{
    m_currentArchive = path;
    setWindowTitle(QString("TARC Archiver - %1").arg(path));
}

void MainWindow::updateArchiveList(const QString& path)
{
    if (!m_model->loadArchive(path)) {
        QMessageBox::warning(this, "Errore", "Impossibile caricare l'archivio: " + path);
        return;
    }
    
    m_currentArchive = path;
    setWindowTitle(QString("TARC Archiver - %1").arg(path));
    
    // Aggiorna la tree view
    m_archiveTree->clear();
    
    // Popola l'albero (implementazione)
}

void MainWindow::setBusy(bool busy)
{
    m_openAction->setEnabled(!busy);
    m_addAction->setEnabled(!busy);
    m_extractAction->setEnabled(!busy);
    m_deleteAction->setEnabled(!busy);
    m_testAction->setEnabled(!busy);
    
    QApplication::setOverrideCursor(busy ? QCursor(Qt::WaitCursor) : QCursor(Qt::ArrowCursor));
}

void MainWindow::updateStatus(const QString& message)
{
    m_statusLabel->setText(message);
}
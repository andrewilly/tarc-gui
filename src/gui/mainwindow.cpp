#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serialPortManager(new SerialPortManager(this))
    , timer(new QTimer(this))
{
    ui->setupUi(this);
    setupConnections();
    
    // Popola la lista delle porte seriali all'avvio
    on_refreshButton_clicked();
    
    // Configura il timer per aggiornare ogni secondo
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);
    timer->start(1000);
    updateTimer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections()
{
    // Connetti i segnali del SerialPortManager agli slot
    connect(serialPortManager, &SerialPortManager::dataReceived,
            this, [this](const QString &data) {
                ui->receivedDataText->append(data);
            });
    
    connect(serialPortManager, &SerialPortManager::errorOccurred,
            this, [this](const QString &error) {
                appendLog("ERRORE: " + error);
                QMessageBox::critical(this, "Errore Seriale", error);
            });
    
    connect(serialPortManager, &SerialPortManager::logMessage,
            this, &MainWindow::appendLog);
}

void MainWindow::appendLog(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->logText->append("[" + timestamp + "] " + message);
}

void MainWindow::on_refreshButton_clicked()
{
    ui->portComboBox->clear();
    
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        ui->portComboBox->addItem(info.portName() + " - " + info.description());
    }
    
    if (ports.isEmpty()) {
        appendLog("Nessuna porta seriale trovata");
    } else {
        appendLog(QString("Trovate %1 porte seriali").arg(ports.count()));
    }
}

void MainWindow::on_connectButton_clicked()
{
    if (serialPortManager->isConnected()) {
        serialPortManager->disconnectPort();
        ui->connectButton->setText("Connetti");
        ui->sendButton->setEnabled(false);
        appendLog("Disconnesso");
    } else {
        QString portName = ui->portComboBox->currentText().split(" - ").first();
        int baudRate = ui->baudRateComboBox->currentText().toInt();
        
        if (serialPortManager->connectToPort(portName, baudRate)) {
            ui->connectButton->setText("Disconnetti");
            ui->sendButton->setEnabled(true);
            appendLog(QString("Connesso a %1 a %2 baud").arg(portName).arg(baudRate));
        } else {
            appendLog("Connessione fallita");
        }
    }
}

void MainWindow::on_sendButton_clicked()
{
    QString data = ui->sendDataLineEdit->text();
    if (!data.isEmpty()) {
        serialPortManager->sendData(data);
        appendLog("Inviato: " + data);
        ui->sendDataLineEdit->clear();
    }
}

void MainWindow::updateTimer()
{
    QString currentTime = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->timeLabel->setText("Ora: " + currentTime);
}

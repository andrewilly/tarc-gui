#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QDateTime>
#include "serialportmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void appendLog(const QString &message);

private slots:
    void on_refreshButton_clicked();
    void on_connectButton_clicked();
    void on_sendButton_clicked();
    void updateTimer();

private:
    Ui::MainWindow *ui;
    SerialPortManager* serialPortManager;
    QTimer* timer;
    void setupConnections();
};

#endif // MAINWINDOW_H

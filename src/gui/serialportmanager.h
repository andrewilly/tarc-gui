#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class SerialPortManager : public QObject
{
    Q_OBJECT

public:
    explicit SerialPortManager(QObject *parent = nullptr);
    ~SerialPortManager();

    bool connectToPort(const QString &portName, int baudRate);
    void disconnectPort();
    bool isConnected() const;
    void sendData(const QString &data);

signals:
    void dataReceived(const QString &data);
    void errorOccurred(const QString &error);
    void logMessage(const QString &message);

private slots:
    void readData();
    void handleError(QSerialPort::SerialPortError error);

private:
    QSerialPort *serialPort;
};

#endif // SERIALPORTMANAGER_H

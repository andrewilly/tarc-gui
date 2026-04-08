#include "serialportmanager.h"

SerialPortManager::SerialPortManager(QObject *parent)
    : QObject(parent)
    , serialPort(new QSerialPort(this))
{
    connect(serialPort, &QSerialPort::readyRead, this, &SerialPortManager::readData);
    connect(serialPort, &QSerialPort::errorOccurred, this, &SerialPortManager::handleError);
}

SerialPortManager::~SerialPortManager()
{
    if (serialPort->isOpen()) {
        serialPort->close();
    }
}

bool SerialPortManager::connectToPort(const QString &portName, int baudRate)
{
    serialPort->setPortName(portName);
    serialPort->setBaudRate(baudRate);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    
    if (serialPort->open(QIODevice::ReadWrite)) {
        emit logMessage("Porta aperta con successo");
        return true;
    } else {
        emit errorOccurred("Impossibile aprire la porta: " + serialPort->errorString());
        return false;
    }
}

void SerialPortManager::disconnectPort()
{
    if (serialPort->isOpen()) {
        serialPort->close();
        emit logMessage("Porta chiusa");
    }
}

bool SerialPortManager::isConnected() const
{
    return serialPort->isOpen();
}

void SerialPortManager::sendData(const QString &data)
{
    if (serialPort->isOpen()) {
        QByteArray byteArray = data.toUtf8();
        byteArray.append('\n'); // Aggiungi newline
        
        qint64 bytesWritten = serialPort->write(byteArray);
        if (bytesWritten == -1) {
            emit errorOccurred("Errore durante l'invio dei dati");
        } else {
            emit logMessage(QString("Inviati %1 bytes").arg(bytesWritten));
        }
    } else {
        emit errorOccurred("Porta non aperta");
    }
}

void SerialPortManager::readData()
{
    QByteArray data = serialPort->readAll();
    QString dataString = QString::fromUtf8(data);
    emit dataReceived(dataString);
    emit logMessage(QString("Ricevuti %1 bytes").arg(data.size()));
}

void SerialPortManager::handleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError && error != QSerialPort::TimeoutError) {
        emit errorOccurred("Errore seriale: " + serialPort->errorString());
    }
}

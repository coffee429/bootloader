#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <QThread>

bool serialConnected = false;
QString hexFile;
QByteArray bootBuffer;                  // data to send to MCU for bootloader
QByteArray recordHexBuffer;             // hex buffer of record adter convert from string


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Bootloader");
    ui->txtFile->setReadOnly(true);
//    ui->btnLoad->setEnabled(false);
}


MainWindow::~MainWindow()
{
    mSerial->close();
    delete ui;
}


void MainWindow::on_btnConnect_clicked()
{
    if(!serialConnected)
    {
        mSerial = new QSerialPort(this);
        mSerial->setPortName("COM19");
        mSerial->setBaudRate(QSerialPort::Baud115200);
        mSerial->setDataBits(QSerialPort::Data8);
        mSerial->setParity(QSerialPort::NoParity);
        mSerial->setStopBits(QSerialPort::OneStop);
        mSerial->setFlowControl(QSerialPort::NoFlowControl);
        mSerial->open(QIODevice::ReadWrite);
        if(mSerial->isOpen())
        {
            ui->btnConnect->setText("Disconnect");
            serialConnected = true;
            ui->btnLoad->setEnabled(true);
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Open error");
            msgBox.exec();
        }
    }
    else
    {
        ui->btnConnect->setText("Connect");
        serialConnected = false;
        mSerial->close();
        ui->txtFile->clear();
        ui->btnLoad->setEnabled(false);
    }
}


void MainWindow::on_btnOpen_clicked()
{
    QString mFilePath = QFileDialog::getOpenFileName(this, tr("Open hex file"), "", tr("Text file(*.hex)"));            // open file path, check file .hex only

    mFile = new QFile(mFilePath);
    if(!mFile->open(QIODevice::ReadOnly | QIODevice::Text))                                                             // open file
    {
        return;
    }

    hexFile = mFile->readAll();                                                                                         // read all file data
    ui->txtFile->setText(hexFile);                                                                                      // set text to ui
}

void MainWindow::on_btnLoad_clicked()
{
    hexFile = hexFile.remove(hexFile.indexOf(":"), hexFile.indexOf("\n") + 1);              // remove 1st record
    hexFile = hexFile.remove(hexFile.lastIndexOf(":"), hexFile.lastIndexOf("\n") + 1);      // remove last record

    int endPos = hexFile.indexOf("\n");                                                     // find position of "\n" character - ending record character
    while (endPos>-1)
    {
//        qInfo() << hexFile.left(endPos);
        getDataRecord((hexFile.left(endPos)).toUtf8());                                    // get byte data to send
        sendFirmwareData(bootBuffer);                                                      // send firmware data to MCU
        bootBuffer.remove(0,16);                                                           // clear data send buffer
        recordHexBuffer.remove(0, 21);                                                     // clear record buffer
        hexFile.remove(0, endPos+1);                                                       // after process 1record(line, remove it
        endPos = hexFile.indexOf("\n");                                                    // find next "\n" character
    }
}



void MainWindow::getDataRecord(QByteArray record)
{
    record = record.remove(0,1);                                                            // remove ":" character
    for(int i=0;i<record.length();i+=2)
    {
      recordHexBuffer.insert(i/2, hexConverter(record.at(i), record.at(i+1)));              // convert record from string to hex and save to buffer
    }
//    qInfo() << recordHexBuffer;

    if(recordHexBuffer.at(3) == 0x00)                                                       // confirm record has firmware data byte to write
    {
        switch (recordHexBuffer.at(0))                                                      // check data size
        {
            case 0x10:                                                                      // 16 byte data to send
                for(int i=0;i<16;i++)
                {
                    bootBuffer.insert(i, recordHexBuffer.at(i+4));
                }
                break;
            case 0x08:                                                                      // 8 byte data to send
                for(int i=0;i<8;i++)
                {
                    bootBuffer.insert(i, recordHexBuffer.at(i+4));
                }
                break;
            case 0x04:                                                                      // 4 byte data to send
                for(int i=0;i<4;i++)
                {
                    bootBuffer.insert(i, recordHexBuffer.at(i+4));
                }
                break;
            case 0x0C:                                                                      // 12 byte data to send
                for(int i=0;i<12;i++)
                {
                    bootBuffer.insert(i, recordHexBuffer.at(i+4));
                }
                break;
        }
        qInfo() << bootBuffer;
    }

}



char MainWindow::hexConverter(char highByte, char lowByte)                                  // convert string to hex type, for example 'A' 'B' -> 0xAB
{
    if(highByte >= '0' && highByte <= '9')			highByte = highByte - '0';
    else if(highByte >= 'A' && highByte <= 'F')		highByte = highByte - 'A' + 10;

    if(lowByte >= '0' && lowByte <= '9')			lowByte = lowByte - '0';
    else if(lowByte >= 'A' && lowByte <= 'F')		lowByte = lowByte - 'A' + 10;

    return (highByte << 4 | lowByte);
}

void MainWindow::sendFirmwareData(QByteArray fwBuffer)
{
    uint8_t dataSize = fwBuffer.length();                                                   // firmware data size in 1 record
    uint8_t _4ByteGroup = 0;                                                                // split data into group of 4 byte to swap, max 16 bytes
    while (_4ByteGroup < dataSize/4)
    {
        uint8_t temp = fwBuffer.at(_4ByteGroup*4);
        fwBuffer[_4ByteGroup*4] = fwBuffer[_4ByteGroup*4 + 3];                              // swap byte 0 & byte 3
        fwBuffer[_4ByteGroup*4 + 3] = temp;
        temp = fwBuffer.at(_4ByteGroup*4 + 1);
        fwBuffer[_4ByteGroup*4 + 1] = fwBuffer[_4ByteGroup*4 + 2];                          // swap byte 1 & byte 2
        fwBuffer[_4ByteGroup*4 + 2] = temp;
        _4ByteGroup ++;
    }
    qInfo() << fwBuffer;
    mSerial->write(":");                                                                    // serial send begin character ":"
    mSerial->write(fwBuffer);                                                             // serial send firmware data
    mSerial->write("\n");                                                                   // serial send end character "\n"
}






























#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <QThread>

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
    QString mFilePath = QFileDialog::getOpenFileName(this, tr("Open hex file"), "", tr("Text file(*.hex)"));    // open file path, check file .hex only

    mFile = new QFile(mFilePath);
    if(!mFile->open(QIODevice::ReadOnly | QIODevice::Text))                                                     // open file
    {
        return;
    }

    hexFile = mFile->readAll();                                                                                 // read all file data
    ui->txtFile->setText(hexFile);                                                                              // set text to ui
    fileProcess(hexFile.toUtf8());                                                                              /* processing file, including check size, convert to hex
                                                                                                                   take data record, swap byte and save buffer to write flash */
    QString totalBytes;                                                                                         // total data byte to send.
    ui->lbSize->setText("Total bytes: " + totalBytes.setNum(getTotalBytes()));
}

void MainWindow::on_btnLoad_clicked()
{
    QByteArray totalSize;
    totalSize.setNum(getTotalBytes());
    mSerial->write(totalSize);                                                              // send to mcu the total number of bytes about to write flash
}

void MainWindow::fileProcess(QByteArray file)
{
    file = file.remove(file.indexOf(":"), file.indexOf("\n") + 1);                          // remove 1st record
    file = file.remove(file.lastIndexOf(":"), file.lastIndexOf("\n") + 1);                  // remove last record


    int endPos = file.indexOf("\n");                                                        // find position of "\n" character - ending record character
    while (endPos>-1)
    {
        getDataRecord((file.left(endPos)));                                                // get byte data to send
        file.remove(0, endPos+1);                                                          // after process 1 record(line), remove it
        endPos = file.indexOf("\n");                                                       // find next "\n" character
    }
    qInfo() << getTotalBytes();
    qInfo() << bootBuffer;
}

uint16_t MainWindow::getTotalBytes()
{
    return totalBytes;                                                                        // get total size
}



void MainWindow::getDataRecord(QByteArray record)                                           // :100000000000022019070208990602089F0602084C
{

    record = record.remove(0,1);                                                            // remove ":" character
    for(int i=0;i<record.length();i+=2)
    {
      recordHexBuffer.insert(i/2, hexConverter(record.at(i), record.at(i+1)));              // convert record from string to hex and save to buffer
    }
//    qInfo() << recordHexBuffer;
//    qInfo() << recordHexBuffer.size();
    if(recordHexBuffer.at(3) == 0x00)                                                       // confirm record has firmware data byte to write
    {
        bootBuffer.insert(bootBuffIdx++, ":");                                              // add start character ":" before send
        for(int i=0;i<recordHexBuffer.size()-5;i++)
        {
            recordDataBuffer.insert(i, recordHexBuffer.at(i+4));                            // get data record in 1 record(line)
        }
        totalBytes += recordDataBuffer.size();                                                // add bytes to total bytes
        addBootloaderData(recordDataBuffer);                                                // call function to add firmware data to bootloader buffer
        bootBuffer.insert(bootBuffIdx++, "\n");                                             // add character "\n"
//        qInfo() << recordDataBuffer;
    }
    recordDataBuffer.remove(0,21);                                                          // clear record data buffer
    recordHexBuffer.remove(0,21);                                                           // clear record hex buffer
}



char MainWindow::hexConverter(char highByte, char lowByte)                                  // convert string to hex type, for example 'A' 'B' -> 0xAB
{
    if(highByte >= '0' && highByte <= '9')			highByte = highByte - '0';
    else if(highByte >= 'A' && highByte <= 'F')		highByte = highByte - 'A' + 10;

    if(lowByte >= '0' && lowByte <= '9')			lowByte = lowByte - '0';
    else if(lowByte >= 'A' && lowByte <= 'F')		lowByte = lowByte - 'A' + 10;

    return (highByte << 4 | lowByte);
}

void MainWindow::addBootloaderData(QByteArray addBuffer)
{
    uint8_t dataSize = addBuffer.length();                                                  // firmware data size in 1 record
    uint8_t _4ByteGroup = 0;                                                                // split data into group of 4 byte to swap, max 16 bytes
    while (_4ByteGroup < dataSize/4)
    {
        uint8_t temp = addBuffer.at(_4ByteGroup*4);
        addBuffer[_4ByteGroup*4] = addBuffer[_4ByteGroup*4 + 3];                            // swap byte 0 & byte 3
        addBuffer[_4ByteGroup*4 + 3] = temp;
        temp = addBuffer.at(_4ByteGroup*4 + 1);
        addBuffer[_4ByteGroup*4 + 1] = addBuffer[_4ByteGroup*4 + 2];                        // swap byte 1 & byte 2
        addBuffer[_4ByteGroup*4 + 2] = temp;
        _4ByteGroup ++;
    }

    for(int i=0;i<addBuffer.length();i++)
    {
        bootBuffer.insert(bootBuffIdx++, addBuffer.at(i));                                  // add firmware data to bootloader buffer
    }
}































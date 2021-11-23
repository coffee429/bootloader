#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

bool serialConnected = false;
QString hexFile;
QByteArray bootData;
QByteArray recordHexBuffer;


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
    QString mFilePath = QFileDialog::getOpenFileName(this, tr("Open hex file"), "", tr("Text file(*.hex)"));

    mFile = new QFile(mFilePath);
    if(!mFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    hexFile = mFile->readAll();
    ui->txtFile->setText(hexFile);
}

void MainWindow::on_btnLoad_clicked()
{
    hexFile = hexFile.remove(hexFile.indexOf(":"), hexFile.indexOf("\n") + 1);              // remove 1st record
    hexFile = hexFile.remove(hexFile.lastIndexOf(":"), hexFile.lastIndexOf("\n") + 1);      // remove last record

    int endPos = hexFile.indexOf("\n");
    while (endPos>-1)
    {
//        qInfo() << hexFile.left(endPos);
        getDataRecord((hexFile.left(endPos)).toUtf8());
        hexFile.remove(0, endPos+1);
        endPos = hexFile.indexOf("\n");
    }
}



void MainWindow::getDataRecord(QByteArray record)
{
    record = record.remove(0,1);                                                // remove ":" character
    for(int i=0;i<record.length();i+=2)
    {
      recordHexBuffer.insert(i/2, hexConverter(record.at(i), record.at(i+1)));       // convert record from string to hex and save to buffer
    }
    qInfo() << recordHexBuffer;
    recordHexBuffer.remove(0, 21);
}



char MainWindow::hexConverter(char highByte, char lowByte)
{
    if(highByte >= '0' && highByte <= '9')			highByte = highByte - '0';
    else if(highByte >= 'A' && highByte <= 'F')		highByte = highByte - 'A' + 10;

    if(lowByte >= '0' && lowByte <= '9')			lowByte = lowByte - '0';
    else if(lowByte >= 'A' && lowByte <= 'F')		lowByte = lowByte - 'A' + 10;

    return (highByte << 4 | lowByte);
}

void MainWindow::swapDataByte()
{

}





























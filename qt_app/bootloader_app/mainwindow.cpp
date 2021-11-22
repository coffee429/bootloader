#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

bool serialConnected = false;
QString hexFile;
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
    int endPos = hexFile.indexOf("\n");
    while (endPos>-1)
    {
        qInfo() << hexFile.left(endPos);
        hexFile.remove(0, endPos+1);
        endPos = hexFile.indexOf("\n");
    }
}





























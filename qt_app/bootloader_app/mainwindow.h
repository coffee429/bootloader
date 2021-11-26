#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnOpen_clicked();

    void on_btnConnect_clicked();

    void on_btnLoad_clicked();

private:
    Ui::MainWindow *ui;

    void fileProcess(QByteArray file);                // processing file, get record, hex convert, get data for bootloader, swap byte, add to bootloader buffer to send

    uint16_t getTotalBytes();                               // get total bytes of data

    void getDataRecord(QByteArray recordToSub);       // get data to send bootloader

    char hexConverter(char highByte, char lowByte);   // convert string to hex

    void addBootloaderData(QByteArray addBuffer);     // add firmware data to bootloader buffer


public:
    QSerialPort *mSerial;
    QFile       *mFile;

    bool serialConnected = false;
    QString hexFile;                        // hex file opened for bootloader
    QByteArray bootBuffer;                  // data to send to MCU for bootloader
    uint16_t bootBuffIdx = 0;               // index in bootloader buffer
    uint16_t totalBytes = 0;
    QByteArray recordHexBuffer;             // hex buffer of record after convert from string
    QByteArray recordDataBuffer;
};
#endif // MAINWINDOW_H

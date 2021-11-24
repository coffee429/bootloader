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

    void getDataRecord(QByteArray recordToSub);

    char hexConverter(char highByte, char lowByte);

    void sendFirmwareData(QByteArray fwBuffer);
public:
    QSerialPort *mSerial;
    QFile       *mFile;
};
#endif // MAINWINDOW_H

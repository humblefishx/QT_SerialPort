
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QLabel>
#include <QTimer>
#include <windows.h>
#include <QString>
#include <dbt.h>
#include <QDebug>
#include<devguid.h>
#include<SetupAPI.h>
#include<InitGuid.h>
#include<QDateTime>

#define delaytime 100

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    void initPort();
    ~MainWindow();

private slots:
        QString StringToHex(QString str);//用于发送时进制转换

        QString StringToHex(unsigned char data);

        void StantardString(QString &str);//规范字符输出

        QString HexToString(QString str);

        void StrCheckCrc(QString str,unsigned char *Crc);

        void on_OrderBuffer_textChanged(const QString &arg1);

        void on_MachineBuffer_textChanged(const QString &arg1);

        void on_DataTypeBuffer_textChanged(const QString &arg1);

        void on_PortBuffer_textChanged(const QString &arg1);

        void on_WebBuffer_textChanged(const QString &arg1);

        void on_sTextBuffer_textChanged();

        void on_InputBuffer_textChanged();

        void on_InitButton_clicked();

        void on_calButton_clicked();

        void AgreementAddCheck(QString &StrSend);

        void HexAddCheck(QString &StrSend);

        void ReadData();//读取数据

        void timeTosend();//定时发送

        //void IsShowSendTime();

        void on_openButton_clicked();

        void on_r_clearButton_clicked();

        void on_s_clearButton_clicked();

        void on_sendButton_clicked();

        void readToHex();//将读取的数据以十六进制显示

        void Mdisplay();

        void on_rTextBuffer_textChanged();

        void Refresh_ComNum();

        void CheckShowTimer();

private:
    bool CheckShowTime;
    Ui::MainWindow *ui;
    QSerialPort *serialPort;
    QStringList PortStringList;
    QTimer *time;           //用于定时发送
    QTimer *RefreshCom;     //用于端口号刷新
    QTimer *ReShowTimer;    //用于接收显示
};

#endif // MAINWINDOW_H

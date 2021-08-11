#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    time = new QTimer(this);
    RefreshCom = new QTimer(this);
    ReShowTimer = new QTimer(this);

    initPort();

    //设置默认值
    ui->BaudBox->setCurrentIndex(0);
    ui->PortBox->setCurrentIndex(1);
    ui->StopBox->setCurrentIndex(0);
    ui->DataBox->setCurrentIndex(3);
    ui->ParityBox->setCurrentIndex(0);

    ui->sendButton->setEnabled(false);
    ui->sHexRadio->setEnabled(false);
    ui->sTextRadio->setEnabled(false);
    ui->sHexRadio->setChecked(false);
    ui->sTextRadio->setChecked(true);

    ui->rTextRadio->setEnabled(false);
    ui->rHexRadio->setEnabled(false);
    ui->rTextRadio->setChecked(true);
    ui->rHexRadio->setChecked(false);

    connect(ui->reSendCheck, &QCheckBox::stateChanged,this, &MainWindow::timeTosend);

    time->connect(time, SIGNAL(timeout()), this, SLOT(on_sendButton_clicked()));

    connect(ui->rHexRadio, &QRadioButton::toggled, this, &MainWindow::Mdisplay);

    RefreshCom->connect(RefreshCom, SIGNAL(timeout()), this, SLOT(Refresh_ComNum()));

    ReShowTimer->connect(ReShowTimer,SIGNAL(timeout()),this,SLOT(CheckShowTimer()));

    RefreshCom->start(500);
}

void MainWindow::initPort()
{

    //获取可用串口
    const auto infos = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo &info : infos)
    {
        QSerialPort serial;
        serial.setPort(info);
        PortStringList += info.portName();
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->PortBox->addItem(info.portName());
            qDebug()<<info.portName();
            serial.close();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* --------------------------------------------------------------------------------------*/
/* -----------------------------------字符处理----------------------------------------------*/
/* --------------------------------------------------------------------------------------*/
QString MainWindow::StringToHex(QString str)
{
    QString str1;
    unsigned char high_hex;
    unsigned char low_hex;
    for(int i=0;i<str.length();i++)
    {
        high_hex = (str.toLatin1().at(i) >> 4) & 0x0f;
        low_hex = str.toLatin1().at(i) & 0x0f;
        if(high_hex > 9)
            high_hex = high_hex + '0' + 7;
        else
            high_hex = high_hex + '0';

        if(low_hex >9)
            low_hex = low_hex + '0' + 7;
        else
            low_hex =  low_hex  + '0';

        str1 +=high_hex;
        str1 +=low_hex;
        str1 +=" ";
    }
    return str1;
}

QString MainWindow::StringToHex(unsigned char data)
{
    QString str1;
    unsigned char high_hex,low_hex;
    high_hex = data/16;
    low_hex = data%16;
    if(high_hex > 9)
        high_hex = high_hex + '0' + 7;
    else
        high_hex = high_hex + '0';

    if(low_hex >9)
        low_hex = low_hex + '0' + 7;
    else
        low_hex =  low_hex  + '0';
    str1 +=  high_hex;
    str1 += low_hex;
    return str1;
}

void MainWindow::StantardString(QString &str)
{
    int num=0;
    str = str.simplified();
    for(int i=0;i<str.length();i++)
    {
        num = i%3;
        switch(num)
        {
            case 0:
                if(str.toStdString().data()[i])
                break;
            case 1:
                if(str.toStdString().data()[i] == ' ')
                 {
                    str = str.insert(i-1,"0");
                    i++;
                }
                break;
            case 2:
                if(str.toStdString().data()[i] != ' ')
                {
                    str = str.insert(i," ");
                    i++;
                }
                break;
        }
    }
}

QString MainWindow::HexToString(QString str)
{
    QString Str1;
    unsigned char data=0,num=0;
 //   str = str.simplified();
 //   str.insert(str.length(),' ');
    str = str.toUpper();

    for(int i=0;i<str.length();i++)
    {
        if((str.toLocal8Bit().data()[i] != ' '))
        {
            if(str.toLocal8Bit().data()[i] == 0x0D)
            {
                if(num)
                {
                    num=0;
                    data = data >>4;
                    Str1 +=  data;
                    data =0;
                }
                Str1 += 0x0D;
                Str1 += 0x0A;
            }
            else
            {
                if(str.toLocal8Bit().data()[i] == 0x0A)
                {
                    if(num)
                    {
                        num=0;
                        data = data >>4;
                        Str1 +=  data;
                        data =0;
                    }
                    Str1 += 0x0D;
                    Str1 += 0x0A;
                }
                else
                {
                    if(str.toLocal8Bit().data()[i]>'9')
                        data |= str.toLocal8Bit().data()[i] - 7 - '0';
                    else
                        data |= str.toLocal8Bit().data()[i] - '0';
                    if(num)
                    {
                        num=0;
                        Str1 +=  data;
                        data =0;
                    }
                    else
                    {
                        if(i == str.length() - 1)
                        {
                            num=0;
                            Str1 +=  data;
                            data =0;
                        }
                        else
                        {
                            data = data <<4;
                            num=1;
                        }
                    }
                }
            }
        }
        else
        {
            if(num)
            {
                num=0;
                data = data >>4;
                Str1 +=  data;
                data =0;
            }
        }
    }
    if(num)
    {
        num=0;
        data = data >>4;
        Str1 +=  data;
        data =0;
    }
    return Str1;
}

void MainWindow::StrCheckCrc(QString str,unsigned char *Crc)
{
    unsigned int sum=0;
    for(int  i=0;i<str.length();i++)
    {
        sum += str.toLatin1().data()[i];
    }
    *Crc = sum & 0xff;
}
/* --------------------------------------------------------------------------------------*/
/* --------------------------------------------------------------------------------------*/


/* --------------------------------------------------------------------------------------*/
/* -----------------------------------和校验----------------------------------------------*/
/* --------------------------------------------------------------------------------------*/
void MainWindow::on_OrderBuffer_textChanged(const QString &arg1)
{
    QString Str;
    bool flag = false;
    Str = arg1.toUpper();
    for(int i=0;i<Str.length();i++)
    {
        if(('0'<=Str.toStdString().data()[i]  && Str.toStdString().data()[i]<='9') ||\
           ('A'<=Str.toStdString().data()[i]  && Str.toStdString().data()[i]<='F') ||\
           (Str.toStdString().data()[i] ==' '))
        {

        }
        else
        {
            Str.remove(i,1);
            i = i-1;
            flag = true;
        }
    }
    if(flag == true)
    {
        if(QMessageBox::warning(this,"警告","字符串不在‘0’-‘9’，‘e/E’-'f/F'之间",QMessageBox::Ok,QMessageBox::Ok) == QMessageBox::Ok)
        {
            ui->OrderBuffer->setText(Str);
        }
    }
}

void MainWindow::on_MachineBuffer_textChanged(const QString &arg1)
{
    QString Str;
    bool flag = false;
    Str = arg1.toUpper();
    for(int i=0;i< Str.length();i++)
    {
        if(('0'<=Str.toStdString().data()[i]  && Str.toStdString().data()[i]<='9') ||\
           ('A'<=Str.toStdString().data()[i]  && Str.toStdString().data()[i]<='F') ||\
           (Str.toStdString().data()[i] ==' '))
        {

        }
        else
        {
            Str.remove(i,1);
            i = i-1;
            flag = true;
        }
    }
    if(flag == true)
    {
        if(QMessageBox::warning(this,"警告","字符串不在‘0’-‘9’，‘e/E’-'f/F'之间",QMessageBox::Ok,QMessageBox::Ok) == QMessageBox::Ok)
        {
            ui->MachineBuffer->setText(Str);
        }
    }
}

void MainWindow::on_DataTypeBuffer_textChanged(const QString &arg1)
{
    QString Str;
    bool flag = false;
    Str = arg1.toUpper();
    for(int i=0;i<Str.length();i++)
    {
        if(('0'<=Str.toStdString().data()[i]  && Str.toStdString().data()[i]<='9') ||\
           ('A'<=Str.toStdString().data()[i]  && Str.toStdString().data()[i]<='F') ||\
           (Str.toStdString().data()[i] ==' '))
        {

        }
        else
        {
            Str.remove(i,1);
            i = i -1;
            flag = true;
        }
    }
    if(flag == true)
    {
        if(QMessageBox::warning(this,"警告","字符串不在‘0’-‘9’，‘e/E’-'f/F'之间",QMessageBox::Ok,QMessageBox::Ok) == QMessageBox::Ok)
        {
            ui->DataTypeBuffer->setText(Str);
        }
    }
}

void MainWindow::on_PortBuffer_textChanged(const QString &arg1)
{
    QString Str;
    bool flag = false;
    Str = arg1.toUpper();
    for(int i=0;i<Str.length();i++)
    {
        if(('0'<=Str.toStdString().data()[i]  && Str.toStdString().data()[i]<='9') &&\
           Str.toStdString().data()[i] != ' ')
        {

        }
        else
        {
            Str.remove(i,1);
            i = i-1;
            flag = true;
        }
    }
    if(flag == true)
    {
        if(QMessageBox::warning(this,"警告","字符串不在‘0’-‘9’之间",QMessageBox::Ok,QMessageBox::Ok) == QMessageBox::Ok)
        {
            ui->PortBuffer->setText(Str);
        }
    }

    QString Str1 = ui->PortBuffer->text();
    Str1  = Str1.simplified();
    if((Str1.toInt()) > 65535)
    {
        int flag1 = QMessageBox::warning(this,"警告","端口号超出65535",QMessageBox::Ok,QMessageBox::Ok);
        if(flag1 == QMessageBox::Ok)
        {
            ui->PortBuffer->clear();
        }
    }
}

void MainWindow::on_WebBuffer_textChanged(const QString &arg1)
{
    QString Str;
    int i;
    bool flag = false;
    Str = arg1;
    for(i=0;i<Str.length();i++)
    {
        if(Str.toStdString().data()[i] ==' ')
        {
            Str.remove(i,1);
            i = i-1;
            flag = true;
        }
    }
    if(flag == true)
    {
        if(QMessageBox::warning(this,"警告","网址中有空格键",QMessageBox::Ok,QMessageBox::Ok) == QMessageBox::Ok)
        {
            ui->WebBuffer->setText(Str);
        }
    }

    if(i > Str.length())
    {
        QString Str1 = ui->WebBuffer->text();
        Str1  = Str1.simplified();
        if(Str1.length() > 27)
        {
            int flag1 = QMessageBox::warning(this,"警告","网址超字符",
                            QMessageBox::Ok,QMessageBox::Ok);
            if(flag1 == QMessageBox::Ok)
            {
                int Num = Str1.count();
                Str1.remove(27,Num-27);
                ui->sTextBuffer->setPlainText(Str1);
            }
        }
    }
}

void MainWindow::on_sTextBuffer_textChanged()
{
    QString Str;
    bool flag = false;
    if (ui->openButton->text() == tr("打开串口"))
        return;
    auto isHexRadio = ui->sHexRadio->isChecked();
    if(isHexRadio)
    {
        Str = ui->sTextBuffer->toPlainText();
        Str = Str.simplified();
        Str = Str.toUpper();
        for(int i=0;i<Str.length();i++)
        {
            if(('0'<=Str.toStdString().data()[i]  && Str.toStdString().data()[i]<='9') ||\
               ('A'<=Str.toStdString().data()[i]  && Str.toStdString().data()[i]<='F') ||\
               (Str.toStdString().data()[i] ==' '))
            {

            }
            else
            {
                Str.remove(i,1);
                i = i-1;
                flag = true;
            }
        }
        if(flag == true)
        {
            if(QMessageBox::warning(this,"警告","字符串不在‘0’-‘9’，‘e/E’-'f/F'之间",QMessageBox::Ok,QMessageBox::Ok) == QMessageBox::Ok)
            {
                ui->sTextBuffer->setPlainText(Str);
            }
        }
    }
}

void MainWindow::on_InputBuffer_textChanged()
{
     QString Str;
     bool flag = false;
     auto IsHex = ui->HexBox->isChecked();
     if(IsHex)
     {
         Str = ui->InputBuffer->toPlainText();
         Str = Str.simplified();
         Str = Str.toUpper();
         for(int i=0;i<Str.length();i++)
         {
             if(('0'<=Str.toStdString().data()[i]  && Str.toStdString().data()[i]<='9') ||\
                ('A'<=Str.toStdString().data()[i]  && Str.toStdString().data()[i]<='F') ||\
                (Str.toStdString().data()[i] ==' '))
             {

             }
             else
             {
                Str.remove(i,1);
                i = i-1;
                flag = true;
             }
         }
         if(flag == true)
         {
             if(QMessageBox::warning(this,"警告","字符串不在‘0’-‘9’，‘e/E’-'f/F'之间",QMessageBox::Ok,QMessageBox::Ok) == QMessageBox::Ok)
             {
                 ui->PortBuffer->setText(Str);
             }
         }
     }
}

void MainWindow::on_InitButton_clicked()
{
    auto Index=ui->tabWidget_2->currentIndex();
    QString str1="00 11";
    QString str2="0A";
    QString str3="95 0A";
    QString str4="jz.glucosemanage.com";
    QString str5="9004";
    switch(Index)
    {
        case 0:
            ui->MachineBuffer->setText(str1);
            ui->OrderBuffer->setText(str2);
            ui->DataTypeBuffer->setText(str3);
            ui->WebBuffer->setText(str4);
            ui->PortBuffer->setText(str5);
            break;
        case 1:
            ui->InputBuffer->clear();
            break;
        default:
            break;
    }
}

void MainWindow::on_calButton_clicked()
{
    QString Str;
    int Num = ui->tabWidget_2->currentIndex();
    Str = Str.simplified();
    switch(Num)
    {
        case 0:
            AgreementAddCheck(Str);
            break;
        case 1:
            HexAddCheck(Str);
            break;
        default:
            break;
    }
    Str = Str.simplified();
    StantardString(Str);
    ui->calTextBuffer->setPlainText(Str);
}

void MainWindow::AgreementAddCheck(QString &StrSend)
{
    QString head="53 4E";
    QString Str,Str1,Str2,Str3;
    int length=0;
    unsigned char Crc=0,num=0;

    Str = ui->MachineBuffer->text();
    StantardString(Str);
    Str1=QString("%1").arg(Str);

    Str = ui->OrderBuffer->text();
    StantardString(Str);
    Str1=QString("%1 %2").arg(Str1).arg(Str);

    Str = ui->DataTypeBuffer->text();
    StantardString(Str);
    Str1=QString("%1 %2").arg(Str1).arg(Str);
    Str1 = Str1.toUpper();
    Str2 = HexToString(Str1);
    length = Str2.length();
    StrCheckCrc(Str2,&Crc);

    Str = ui->WebBuffer->text();
    Str = Str.simplified();
    Str3=QString("%1").arg(Str);

    Str = ui->PortBuffer->text();
    Str =Str.simplified();
    Str2=QString("%1,%2").arg(Str3).arg(Str);

    length = length + Str2.length() +1;
    StrCheckCrc(Str2,&num);
    Crc = Crc + num + length ;

    Str=StringToHex(Str2);
    StrSend=QString("%1 %2 %3 %4%5").arg(head).arg(StringToHex((unsigned char)length)).arg(Str1).arg(Str).arg(StringToHex(Crc));
}

void MainWindow::HexAddCheck(QString &StrSend)
{
    QString Str=ui->InputBuffer->toPlainText();
    unsigned char Crc=0;
    auto HexCheck=ui->HexBox->isChecked();
    if(!Str.isEmpty())
    {
        if(HexCheck)
        {
            StrCheckCrc(HexToString(Str),&Crc);
        }
        else
        {
            StrCheckCrc(Str,&Crc);
            Str = StringToHex(Str);
        }
        StrSend = QString("%1 %2").arg(Str).arg(StringToHex(Crc));
    }
}

/* --------------------------------------------------------------------------------------*/
/* -----------------------------------串口相关---------------------------------------------*/
/* --------------------------------------------------------------------------------------*/
void MainWindow::ReadData()
{
  //  static int reNum = 0;
    QByteArray buf;
    buf = serialPort->readAll();

    if (!buf.isEmpty())
    {
  //      reNum += buf.size();
        if(ReShowTimer->isActive())
        {
            ReShowTimer->stop();
        }
        ReShowTimer->start(20);

        QString myStrTemp = QString::fromLocal8Bit(buf); //支持中文显示

        if(CheckShowTime)
        {
            CheckShowTime = false;
            myStrTemp=myStrTemp.insert(0,"收<-:");
        }

        QString str = ui->rTextBuffer->toPlainText();
        str +=myStrTemp;
        ui->rTextBuffer->clear();
        ui->rTextBuffer->insertPlainText(str);

        auto isShowTime = ui->TimeButton->isChecked();
        if(isShowTime)
        {
            QDateTime current_date_time =QDateTime::currentDateTime();
            QString current_date =current_date_time.toString("[yyyy.MM.dd hh:mm:ss.zzz]");
            ui->rTextBuffer->insertPlainText(current_date);
        }
    }
    buf.clear();
}

void MainWindow::timeTosend()
{
    if(ui->reSendCheck->isChecked())
    {
        if(time->isActive())
        {
            return;
        }
        else
        {
            int ms = ui->spinBox->value();
            time->start(ms);
        }
    }
    else
    {
        if(time->isActive())
        {
            time->stop();
        }
        else
        {
            return;
        }
    }
}

void MainWindow::on_openButton_clicked()
{
    if (ui->openButton->text() == tr("打开串口"))
    {
        serialPort = new QSerialPort;

        serialPort->setPortName(ui->PortBox->currentText());

        if(serialPort->open(QIODevice::ReadWrite))
        {
            switch (ui->BaudBox->currentIndex()) {
            case 0:
                serialPort->setBaudRate(QSerialPort::Baud115200);
                break;
            case 1:
                serialPort->setBaudRate(QSerialPort::Baud57600);
                break;
            case 2:
                serialPort->setBaudRate(QSerialPort::Baud38400);
                break;
            case 3:
                serialPort->setBaudRate(QSerialPort::Baud19200);
                break;
            case 4:
                serialPort->setBaudRate(QSerialPort::Baud9600);
                break;
            case 5:
                serialPort->setBaudRate(QSerialPort::Baud4800);
                break;
            case 6:
                serialPort->setBaudRate(QSerialPort::Baud1200);
                break;
            default:
                serialPort->setBaudRate(QSerialPort::Baud115200);
                break;
            }

            switch (ui->StopBox->currentIndex()) {
            case 0:
                serialPort->setStopBits(QSerialPort::OneStop);
                break;
            case 1:
                serialPort->setStopBits(QSerialPort::OneAndHalfStop);
                break;
            case 2:
                serialPort->setStopBits(QSerialPort::TwoStop);
                break;
            default:
                serialPort->setStopBits(QSerialPort::OneStop);
                break;
            }

            switch (ui->DataBox->currentIndex()) {
            case 0:
                serialPort->setDataBits(QSerialPort::Data5);
                break;
            case 1:
                serialPort->setDataBits(QSerialPort::Data6);
                break;
            case 2:
                serialPort->setDataBits(QSerialPort::Data7);
                break;
            case 3:
                serialPort->setDataBits(QSerialPort::Data8);
                break;
            default:
                serialPort->setDataBits(QSerialPort::Data8);
                break;
            }

            switch (ui->ParityBox->currentIndex()) {
            case 0:
                serialPort->setParity(QSerialPort::NoParity);
                break;
            case 1:
                serialPort->setParity(QSerialPort::OddParity);
                break;
            case 2:
                serialPort->setParity(QSerialPort::MarkParity);
                break;
            default:
                serialPort->setParity(QSerialPort::NoParity);
                break;
            }

            ui->openButton->setText(tr("关闭串口"));
            ui->rTextRadio->setEnabled(true);
            ui->rHexRadio->setEnabled(true);
            ui->PortBox->setEnabled(false);
            ui->BaudBox->setEnabled(false);
            ui->StopBox->setEnabled(false);
            ui->DataBox->setEnabled(false);
            ui->ParityBox->setEnabled(false);
            ui->sendButton->setEnabled(true);
            ui->sTextRadio->setEnabled(true);
            ui->sHexRadio->setEnabled(true);

            ui->rTextRadio->setChecked(true);
            ui->rHexRadio->setChecked(false);
            ui->sTextRadio->setChecked(true);
            ui->sHexRadio->setChecked(false);
            connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::ReadData);
//            connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readToHex);
        }
        else
        {
            QMessageBox::critical(this, tr("Error"), serialPort->errorString());
        }
    }
    else
    {
        serialPort->clear();
        serialPort->close();
        serialPort->deleteLater();

        ui->sendButton->setEnabled(false);
        ui->openButton->setText(tr("打开串口"));
        ui->rTextRadio->setEnabled(false);
        ui->rHexRadio->setEnabled(false);
        ui->PortBox->setEnabled(true);
        ui->BaudBox->setEnabled(true);
        ui->StopBox->setEnabled(true);
        ui->DataBox->setEnabled(true);
        ui->ParityBox->setEnabled(true);
        ui->sHexRadio->setEnabled(false);
        ui->sTextRadio->setEnabled(false);
    }
}

void MainWindow::on_r_clearButton_clicked()
{
    ui->rTextBuffer->clear();
}

void MainWindow::on_s_clearButton_clicked()
{
    ui->sTextBuffer->clear();
}

void MainWindow::on_sendButton_clicked()
{
    //Latin1是ISO-8859-1的别名，有些环境下写作Latin-1。ISO-8859-1编码是单字节编码，向下兼容ASCII
   //其编码范围是0x00-0xFF，0x00-0x7F之间完全和ASCII一致，0x80-0x9F之间是控制字符，0xA0-0xFF之间是文字符号。
//    QString test = ui->textEdit->toPlainText();
//    qDebug()<<test<<endl;
//    serialPort->write(test.toLocal8Bit());
    QString str = ui->sTextBuffer->toPlainText();
    if(!str.isEmpty())
    {
        auto isHexSend = ui->sHexRadio->isChecked();
        if(isHexSend)
        {
            str = HexToString(str);

            auto issLineChange = ui ->sLinechange ->isChecked();
            if(issLineChange)
                str.append("\r\n");//发送新行
            if(serialPort->write(str.toLatin1().data())<0)
            {
                QMessageBox::critical(this, tr("Error"), serialPort->errorString());
            }
        }
        else
        {
            auto issLineChange = ui ->sLinechange ->isChecked();
            if(issLineChange)
                str.append("\r\n");//发送新行

            if(serialPort->write(str.toLocal8Bit())<0)
            {
                QMessageBox::critical(this, tr("Error"), serialPort->errorString());
            }
        }

        auto isredisplay = ui->reDisplay->isChecked();
        if(isredisplay)
        {
            auto isHexRead = ui->rHexRadio->isChecked();
            if(isHexRead)
                str = StringToHex(str);
            str.insert(0,"发->:");

            ui->rTextBuffer->insertPlainText(str);

            auto isShowTime = ui->TimeButton->isChecked();
            if(isShowTime)
            {
                QDateTime current_date_time =QDateTime::currentDateTime();
                QString current_date =current_date_time.toString("[yyyy.MM.dd hh:mm:ss.zzz]");
                ui->rTextBuffer->insertPlainText(current_date);
            }
        }
    }
}

void MainWindow::readToHex()
{
    QByteArray temp = serialPort->readAll();
    QDataStream out(&temp,QIODevice::ReadWrite);    //将字节数组读入

    if(ReShowTimer->isActive())
    {
        ReShowTimer->stop();
    }
    ReShowTimer->start(10);

//    auto ischangeLine = ui->LineChangeButton->isChecked();
//    if(ischangeLine)
//        ui->rTextBuffer->append("");
    if(CheckShowTime)
    {
        CheckShowTime = false;
        ui->rTextBuffer->insertPlainText("收<-:");
    }

    while(!out.atEnd())
    {
           qint8 outChar = 0;
           out>>outChar;   //每字节填充一次，直到结束
           //十六进制的转换
           QString str = QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0'));
           ui->rTextBuffer->insertPlainText(str.toUpper());//大写
           ui->rTextBuffer->insertPlainText(" ");//每发送两个字符后添加一个空格
           ui->rTextBuffer->moveCursor(QTextCursor::End);
    }
}

void MainWindow::Mdisplay()
{
    if(ui->rHexRadio->isChecked())
    {
        disconnect(serialPort, &QSerialPort::readyRead, this, &MainWindow::ReadData);
        connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readToHex);
    }
    else
    {
        connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::ReadData);
        disconnect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readToHex);
    }
}

void MainWindow::on_rTextBuffer_textChanged()
{
    ui->rTextBuffer->moveCursor(QTextCursor::End);
}

void MainWindow::Refresh_ComNum()
{
    auto IsOpen = ui->PortBox->isEnabled();
    if(IsOpen)
    {
        QStringList newPortStringList;
        const auto infos = QSerialPortInfo::availablePorts();
        for(const QSerialPortInfo &info : infos)
        {
            newPortStringList += info.portName();
        }

        if(newPortStringList.size() != PortStringList.size())
        {
            PortStringList = newPortStringList;
            ui->PortBox->clear();
            ui->PortBox->addItems(PortStringList);
        }
    }
}

void MainWindow::CheckShowTimer()
{
    CheckShowTime = true;
    auto isShowTime = ui->TimeButton->isChecked();
    if(isShowTime)
    {
        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("[yyyy.MM.dd hh:mm:ss.zzz]");
        ui->rTextBuffer->insertPlainText(current_date);
    }

    auto ischangeLine = ui->LineChangeButton->isChecked();
    if(ischangeLine)
        ui->rTextBuffer->append("");

    ReShowTimer->stop();
}

/* --------------------------------------------------------------------------------------*/
/* --------------------------------------------------------------------------------------*/

#include "dialog.h"
#include "ui_dialog.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QtWidgets>


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
     //background.load("://background.png");


       //加入的代码
        QObject::connect(ui->green_slider,SIGNAL(valueChanged(int)),this,SLOT(on_red_slider_valueChanged(int)));
        QObject::connect(ui->blue_slider,SIGNAL(valueChanged(int)),this,SLOT(on_red_slider_valueChanged(int)));
        //将不同滑动条发出的valueChanged(int)信号都与第一个槽函数关联，这样四个中任意一个值改变都会发送信号来调用槽函数。
    arduino_is_available = false;
    arduino_port_name = "";
    arduino = new QSerialPort;

    /*
    qDebug() << "Number of available ports: " << QSerialPortInfo::availablePorts().length();
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        qDebug() << "Has vendor ID: " << serialPortInfo.hasVendorIdentifier();
        if(serialPortInfo.hasVendorIdentifier()){
            qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
        }
        qDebug() << "Has Product ID: " << serialPortInfo.hasProductIdentifier();
        if(serialPortInfo.hasProductIdentifier()){
            qDebug() << "Product ID: " << serialPortInfo.productIdentifier();
        }
    }
    */

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
            if(serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id){
                if(serialPortInfo.productIdentifier() == arduino_uno_product_id){
                    arduino_port_name = serialPortInfo.portName();
                    arduino_is_available = true;
                }
            }
        }
    }

    if(arduino_is_available){
        // open and configure the serialport
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::WriteOnly);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
    }else{
        // give error message if not available
        QMessageBox::warning(this, "Port error", "Couldn't find the Arduino!");
    }
}

Dialog::~Dialog()
{
    if(arduino->isOpen()){
        arduino->close();
    }
    delete ui;
}

void Dialog::on_red_slider_valueChanged(int value)
{
    ui->red_value_label->setText(QString("<span style=\" font-size:18pt; font-weight:600;\">%1</span>").arg(value));
    Dialog::updateRGB(QString("r%1").arg(value));
    qDebug() << value;





    Q_UNUSED(value);
    QColor color;
    //获取四个滚动条的值
    int R = ui->red_slider->value();
    int G = ui->green_slider->value();
    int B = ui->blue_slider->value();


    // 输出调试信息 qDebug() << "R="<<R<<"  G="<<G<<"  B="<<B<<" A="<<A;
    color.setRgb(R,G,B);
    QString strClr = color.name(); // "#XXXXX" strClr会得到color 十六进制表示的数据
    QString strStyleSheet = QString("background-color: %1").arg( strClr ); //%1 类似于printf()中的占位符  使用arg拼接字符串
    ui->textEdit->setStyleSheet( strStyleSheet ); //改变textEdit样式表
}

void Dialog::on_green_slider_valueChanged(int value)
{
    ui->green_value_label->setText(QString("<span style=\" font-size:18pt; font-weight:600;\">%1</span>").arg(value));
    Dialog::updateRGB(QString("g%1").arg(value));
    qDebug() << value;
}

void Dialog::on_blue_slider_valueChanged(int value)
{
    ui->blue_value_label->setText(QString("<span style=\" font-size:18pt; font-weight:600;\">%1</span>").arg(value));
    Dialog::updateRGB(QString("b%1").arg(value));
    qDebug() << value;
}

void Dialog::updateRGB(QString command)
{
    if(arduino->isWritable()){
        arduino->write(command.toStdString().c_str());
    }else{
        qDebug() << "Couldn't write to serial!";
    }
}

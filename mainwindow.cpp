/*************************************************************************
**  All rights reserved by Yantai XTD test technology co., LTD.			**
**																		**
**                          Author: Dong Shengwei						**
**          				Date: 2015-03-07                            **
*************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QByteArray>
#include <QLabel>
#include <QObject>
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QComboBox>
#include <QLineEdit>
#include <QString>
#include <QByteArray>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("多源数据采集软件"));
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(handleData()));
    init();
}

MainWindow::~MainWindow()
{
//    for (int i = 0; i < CHANNELMAX; i ++)
//    {
//        delete inData[i];
//    }
//    delete outData;
    delete ui;
}

//保存配置(将用户当前配置保存到自定义配置文件中)
void MainWindow::on_saveCfgButton_clicked()
{
    customConfig = new QSettings("customConfig.txt", QSettings::IniFormat);
    customConfig->setValue("main/channelCount", ui->countChnSpinBox->text());
    customConfig->setValue("main/outputCOM", ui->outPutPortComboBox->currentText());

    for (int i = 0; i < ui->countChnSpinBox->text().toInt(); i ++)
    {
        customConfig->setValue(QString("channel-%1/dataSrc").arg(i+1), dataSrcBox[i]->currentIndex());
        customConfig->setValue(QString("channel-%1/inputCOM").arg(i+1), inputCOMBox[i]->currentText());
        if (ADCSwitchBox[i]->isVisible())
            customConfig->setValue(QString("channel-%1/ADCSwitch").arg(i+1), ADCSwitchBox[i]->currentText());
        else
            customConfig->setValue(QString("channel-%1/ADCSwitch").arg(i+1), QString("DCV"));
    }
    delete customConfig;
}

//重置配置(从出厂配置文件中读取初始化数值)
void MainWindow::on_resetConfigButton_clicked()
{
    defaultConfig = new QSettings("defaultConfig.txt", QSettings::IniFormat);
    ui->countChnSpinBox->setValue(defaultConfig->value("main/channelCount").toInt());
    ui->outPutPortComboBox->setCurrentText(defaultConfig->value("main/outputCOM").toString());

    for (int i = 0; i < ui->countChnSpinBox->text().toInt(); i ++)
    {
        qDebug() << "111111111111111: " << i;
        dataSrcBox[i]->setCurrentIndex(defaultConfig->value(QString("channel-%1/dataSrc").arg(i+1)).toInt());
        qDebug() << "333333333333333: " << defaultConfig->value(QString("channel-%1/inputCOM").arg(i+1)).value<QString>();
        inputCOMBox[i]->setCurrentText(defaultConfig->value(QString("channel-%1/inputCOM").arg(i+1)).value<QString>());
        qDebug() << "333333333333333: " << defaultConfig->value(QString("channel-%1/ADCSwitch").arg(i+1)).value<QString>();
        if (ADCSwitchBox[i]->isVisible())
            ADCSwitchBox[i]->setCurrentText(defaultConfig->value(QString("channel-%1/ADCSwitch").arg(i+1)).value<QString>());
        qDebug() << i << " " << dataSrcBox[i]->currentIndex() << " " << inputCOMBox[i]->currentText()
                 << " " << ADCSwitchBox[i]->currentText();
    }
    delete defaultConfig;
}

void MainWindow::on_countChnSpinBox_valueChanged(int)
{
    display();
    showData(ui->channelTab->count()-1);
}

void MainWindow::on_startButton_clicked()
{
    dataRun();

    if (true == runFlag)
    {
        for (int i = 0; i < CHANNELMAX; i ++)
            connect(ADCSwitchBox[i], SIGNAL(currentTextChanged(QString)), inData[i], SLOT(changADC(QString)));
        for (int i = 0; i < ui->countChnSpinBox->value(); i ++)
        {
            inData[i]->setCOMName(inputCOMBox[i]->currentText());
            inData[i]->setDataSrc(dataSrcBox[i]->currentText());
            inData[i]->init();
            if (QStringLiteral("固纬数字万用表") == dataSrcBox[i]->currentText())
            {
                inData[i]->run(ADCSwitchBox[i]->currentText());
            }
        }
        outData->setCOMName(ui->outPutPortComboBox->currentText());
        outData->initOutputCOM();
    } else {
        for (int i = 0; i < CHANNELMAX; i ++)
            disconnect(ADCSwitchBox[i], SIGNAL(currentTextChanged(QString)), inData[i], SLOT(changADC(QString)));
    }
}

void MainWindow::handleData()
{
    QByteArray sendBuf("B");
    for(int i = 0; i < ui->countChnSpinBox->value(); i ++)
    {
        dataStr.setNum(inData[i]->getData());

        sendBuf.append(dataStr);
        if (i < ui->countChnSpinBox->value()-1)
            sendBuf.append(",");

        valueDisplay[i]->setText(dataStr);

        if(0 == inData[i]->getStd())
            unitLabel[i]->setText("mm");
        else
            unitLabel[i]->setText("inch");
    }
    sendBuf.append("E");
    //    qDebug() << "the result: " << sendBuf.data();
    outData->setData(sendBuf);
}

void MainWindow::showInterface()
{
    showData(ui->channelTab->currentIndex());
}

void MainWindow::showData(int i)
{
    if (0 <= i)
    {
        if (QStringLiteral("固纬数字万用表") == dataSrcBox[i]->currentText())
        {
            valueLabel[i]->setText(QStringLiteral(" 电  压 "));
            unitLabel[i]->setText("V ");
            ADCSwitchLabel[i]->setText(QStringLiteral("交 直 流"));
            ADCSwitchBox[i]->setHidden(false);
        } else {
            valueLabel[i]->setText(QStringLiteral(" 高  度 "));
            if(0 == inData[i]->getStd())
                unitLabel[i]->setText("mm");
            else
                unitLabel[i]->setText("inch");
            ADCSwitchLabel[i]->setText("");
            ADCSwitchBox[i]->setHidden(true);
        }
    }
}

void MainWindow::init()
{
    runFlag = false;
    dataStr = "";
    layoutTabWidget();
    initConfig();
    showData(ui->channelTab->count()-1);
}

//选项卡的布局
void MainWindow::layoutTabWidget()
{
    for(int i =  0; i < CHANNELMAX; i ++)
    {
        dataSrcLabel[i] = new QLabel(QStringLiteral("数据来源"));
        inputCOMLabel[i] = new QLabel(QStringLiteral("输入端口"));
        dataSrcLabel[i] = new QLabel(QStringLiteral("数据来源"));
        valueLabel[i] = new QLabel(QStringLiteral(" 数  值 "));
        ADCSwitchLabel[i] = new QLabel(QStringLiteral("交 直 流"));
        unitLabel[i] = new QLabel(QString("V "));
        nullLabel1[i] = new QLabel(QString(" "));
        nullLabel2[i] = new QLabel(QString(" "));

        dataSrcBox[i] = new QComboBox();
        dataSrcBox[i]->addItem(QStringLiteral("三和数显指示表"));
        dataSrcBox[i]->addItem(QStringLiteral("固纬数字万用表"));
        dataSrcBox[i]->setCurrentText(QStringLiteral("三和数显指示表"));
        inputCOMBox[i] = new QComboBox();
        for (int j = 0; j < 20; j ++)
            inputCOMBox[i]->addItem(QString("COM%1").arg(j+1));
        ADCSwitchBox[i] = new QComboBox();
        ADCSwitchBox[i]->addItem("DCV");
        ADCSwitchBox[i]->addItem("ACV");
        valueDisplay[i] = new QLineEdit();
        valueDisplay[i]->setText(QString("%1").arg("0", 6));

        tab[i] = new QWidget();

        layoutInputCOM[i] = new QHBoxLayout;
        layoutValue[i] = new QHBoxLayout;
        layoutADC[i] = new QHBoxLayout;
        layoutMain[i] = new QHBoxLayout;
        layoutLabel[i] = new QVBoxLayout;
        layoutOption[i] = new QVBoxLayout;
        layoutTab[i] = new QGridLayout;

        spacer1[i] = new QSpacerItem(20, 0);
        spacer2[i] = new QSpacerItem(20, 0);

        layoutLabel[i]->addWidget(dataSrcLabel[i]);
        layoutLabel[i]->addWidget(inputCOMLabel[i]);
        layoutLabel[i]->addWidget(valueLabel[i]);
        layoutLabel[i]->addWidget(ADCSwitchLabel[i]);

        layoutInputCOM[i]->addWidget(inputCOMBox[i]);
        layoutInputCOM[i]->addWidget(nullLabel1[i]);

        layoutValue[i]->addWidget(valueDisplay[i]);
        layoutValue[i]->addWidget(unitLabel[i]);

        layoutADC[i]->addWidget(ADCSwitchBox[i]);
        layoutADC[i]->addWidget(nullLabel2[i]);

        layoutOption[i]->addWidget(dataSrcBox[i]);
        layoutOption[i]->addLayout(layoutInputCOM[i]);
        layoutOption[i]->addLayout(layoutValue[i]);
        layoutOption[i]->addLayout(layoutADC[i]);

        layoutMain[i]->addLayout(layoutLabel[i]);
        layoutMain[i]->addLayout(layoutOption[i]);

        tab[i]->setLayout(layoutMain[i]);

        inData[i] = new InputData;

        connect(dataSrcBox[i], SIGNAL(currentIndexChanged(int)), this, SLOT(showInterface()));
    }
    outData = new OutputData;
    for (int j = 0; j < 20; j ++)
        ui->outPutPortComboBox->addItem(QString("COM%1").arg(j+1));
    for (int i = 0; i < ui->countChnSpinBox->value(); i ++)
        ui->channelTab->addTab(tab[i], QStringLiteral("通道%1").arg(i+1));
}
//界面显示
void MainWindow::display()
{
    if (ui->channelTab->count() < ui->countChnSpinBox->value())
    {
        for (int i = ui->channelTab->count(); i <= ui->countChnSpinBox->value(); i ++)
        {
            ui->channelTab->addTab(tab[i-1],QStringLiteral("通道%1").arg(i));
        }
    } else if (ui->channelTab->count() > ui->countChnSpinBox->value()) {
        while(ui->channelTab->count() > ui->countChnSpinBox->value())
        {
            ui->channelTab->removeTab(ui->channelTab->count()-1);
        }
    }
}

//初始化配置
void MainWindow::initConfig()
{
    if(QFile("customConfig.txt").exists()) {
        config = new QSettings("customConfig.txt", QSettings::IniFormat);
    } else {
        config = new QSettings("defaultConfig.ini", QSettings::IniFormat);
    }
    ui->countChnSpinBox->setValue(config->value("main/channelCount").toInt());
    ui->outPutPortComboBox->setCurrentText(config->value("main/outputCOM").toString());

    for (int i = 0; i < ui->countChnSpinBox->text().toInt(); i ++)
    {
        dataSrcBox[i]->setCurrentIndex(config->value(QString("channel-%1/dataSrc").arg(i+1)).toInt());
        inputCOMBox[i]->setCurrentText(config->value(QString("channel-%1/inputCOM").arg(i+1)).toString());
        ADCSwitchBox[i]->setCurrentText(config->value(QString("channel-%1/ADCSwitch").arg(i+1)).toString());
    }
    delete config;
}

void MainWindow::dataRun()
{
    runFlag = !runFlag;
    bool checkOk = true;

    if( true == runFlag) {
        ui->startButton->setText(QStringLiteral("停止"));
    } else {
        ui->startButton->setText(QStringLiteral("开始"));
    }

    if (true == runFlag)
    {
        for (int i = 0; i < ui->countChnSpinBox->value(); i ++)
        {/*
            inputDataCOM[i] = new Win_QextSerialPort(inputCOMBox[i]->currentText(),
                                                     *inputCOMSet[i], QextSerialBase::EventDriven);
            inputDataCOM[i]->open(QIODevice::ReadWrite);*/

            //            connect(inData[i].inputDataCOM, SIGNAL(readyRead()), this, SLOT(inData[i].readInputData()));

            for (int j = 0; j < i; j ++)
            {
                if (inputCOMBox[j]->currentText() == inputCOMBox[i]->currentText())
                {
                    QMessageBox msgBox;
                    msgBox.setText(QStringLiteral("串口有冲突,请检查"));
                    msgBox.exec();
                    checkOk = false;
                    break;
                }
            }
            if (false == checkOk)
                break;
        }

        timer->start(500);

    } else {
        timer->stop();
    }
}

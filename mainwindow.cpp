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
#include <qmath.h>
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
    connect(timer, SIGNAL(timeout()), this, SLOT(sendOutputData()));
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//保存配置(将用户当前配置保存到自定义配置文件中)
void MainWindow::on_saveCfgButton_clicked()
{
    customConfig = new QSettings("customConfig.ini", QSettings::IniFormat);
    customConfig->setValue("main/channelCount", ui->countChnSpinBox->text());
    customConfig->setValue("main/outputCOM", ui->outPutPortComboBox->currentText());

    for (int i = 0; i < ui->countChnSpinBox->text().toInt(); i ++)
    {
        customConfig->setValue(QString("channel-%1/dataSrc").arg(i+1), dataSrcBox[i]->currentIndex());
        customConfig->setValue(QString("channel-%1/inputCOM").arg(i+1), inputCOMBox[i]->currentText());
        customConfig->setValue(QString("channel-%1/ADCSwitch").arg(i+1), ADCSwitchBox[i]->currentText());
    }
    delete customConfig;
}

//重置配置(从出厂配置文件中读取初始化数值)
void MainWindow::on_resetConfigButton_clicked()
{
    defaultConfig = new QSettings("defaultConfig.ini", QSettings::IniFormat);
    ui->countChnSpinBox->setValue(defaultConfig->value("main/channelCount").toInt());
    ui->outPutPortComboBox->setCurrentText(defaultConfig->value("main/outputCOM").toString());

    for (int i = 0; i < ui->countChnSpinBox->text().toInt(); i ++)
    {
        dataSrcBox[i]->setCurrentIndex(defaultConfig->value(QString("channel-%1/dataSrc").arg(i+1)).toInt());
        inputCOMBox[i]->setCurrentText(defaultConfig->value(QString("channel-%1/inputCOM").arg(i+1)).toString());
        ADCSwitchBox[i]->setCurrentText(defaultConfig->value(QString("channel-%1/ADCSwitch").arg(i+1)).toString());
    }
    delete defaultConfig;
}

void MainWindow::on_countChnSpinBox_valueChanged(int)
{
    display();
}

void MainWindow::on_startButton_clicked()
{
    dataRun();

    if (true == runFlag)
    {
        valueFlag = false;
        beginFlag = false;
        for (int i = 0; i < ui->countChnSpinBox->value(); i ++)
        {
            if (QStringLiteral("固纬数字万用表") == dataSrcBox[i]->currentText())
            {
                qDebug() << "gw00 " << GDM_connect_cmd1.data();

                sendGDMData(inputDataCOM[i], GDM_connect_cmd1);
            }
        }
    }
}

void MainWindow::readInputData()
{
    qDebug() << "read input data";
    for (int i = 0; i < ui->countChnSpinBox->value(); i ++)
    {
        inputBuf[i] = inputDataCOM[i]->readAll();
//        qDebug() << "i: " << i << " data: " << inputBuf[i].data();
//        qDebug() << "i: " << i << " data: " << QByteArray::fromHex(inputBuf[i].data());
        updateInputData(inputDataCOM[i], inputBuf[i].toHex(), i);
    }
}

void MainWindow::sendGDMData(Win_QextSerialPort *GDMCOM, QByteArray hexStr)
{
    qDebug() << "sendGDMData: " << hexStr.data();
    GDMCOM->write(hexStr, hexStr.length());
}

void MainWindow::sendOutputData()
{
    QByteArray sendBuf("B");
    for(int i = 0; i < ui->countChnSpinBox->value(); i ++)
    {
        sendBuf.append(data[i]);
        if (i < ui->countChnSpinBox->value()-1)
            sendBuf.append(",");
    }
    sendBuf.append("E");
    outputDataCOM->write(sendBuf, sendBuf.length());
}

void MainWindow::init()
{
    runFlag = false;
    initGDMData();
    layoutTabWidget();

    for (int i = 0; i < CHANNELMAX; i ++)
    {
        connect(inputCOMBox[i], SIGNAL(currentIndexChanged(int)), this, SLOT(initCOM()));
        connect(dataSrcBox[i], SIGNAL(currentIndexChanged(int)), this, SLOT(initCOM()));
        connect(ADCSwitchBox[i], SIGNAL(currentIndexChanged(int)), this, SLOT(initCOM()));
    }

    connect(ui->outPutPortComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(initCOM()));

    initCOM();
    initConfig();
}

void MainWindow::initGDMData()
{
    GDM_connect_cmd1 = QByteArray::fromHex(GDM_CONNECT_CMD1);
    GDM_connect_cmd2 = QByteArray::fromHex(GDM_CONNECT_CMD2);
    GDM_connect_response1 = QByteArray::fromHex(GDM_CONNECT_RESPONSE1);
    GDM_connect_response2 = QByteArray::fromHex(GDM_CONNECT_RESPONSE2);
    GDM_connect_done = QByteArray::fromHex(GDM_CONNECT_DONE);
    GDM_connect_done_response = QByteArray::fromHex(GDM_CONNECT_DONE_RESPONSE);
    GDM_switchto_dcv = QByteArray::fromHex(GDM_SWITCHTO_DCV);
    GDM_switchto_dcv_done = QByteArray::fromHex(GDM_SWITCHTO_DCV_DONE);
    GDM_switchto_acv = QByteArray::fromHex(GDM_SWITCHTO_ACV);
    GDM_switchto_acv_done = QByteArray::fromHex(GDM_SWITCHTO_ACV_DONE);
    GDM_get_data = QByteArray::fromHex(GDM_GET_DATA);
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
        unitLabel[i] = new QLabel(QString("V"));

        dataSrcBox[i] = new QComboBox();
        dataSrcBox[i]->addItem(QStringLiteral("三和数显指示表"));
        dataSrcBox[i]->addItem(QStringLiteral("固纬数字万用表"));
        dataSrcBox[i]->setCurrentText(QStringLiteral("三和数显指示表"));
        inputCOMBox[i] = new QComboBox();
        for (int j = 0; j < 20; j ++)
            inputCOMBox[i]->addItem(QString("COM%1").arg(j+1));
        ADCSwitchBox[i] = new QComboBox();
        ADCSwitchBox[i]->addItem("ACV");
        ADCSwitchBox[i]->addItem("DCV");
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
        layoutInputCOM[i]->addItem(spacer1[i]);

        layoutValue[i]->addWidget(valueDisplay[i]);
        layoutValue[i]->addWidget(unitLabel[i]);

        layoutADC[i]->addWidget(ADCSwitchBox[i]);
        layoutADC[i]->addItem(spacer2[i]);

        layoutOption[i]->addWidget(dataSrcBox[i]);
        layoutOption[i]->addLayout(layoutInputCOM[i]);
        layoutOption[i]->addLayout(layoutValue[i]);
        layoutOption[i]->addLayout(layoutADC[i]);

        layoutMain[i]->addLayout(layoutLabel[i]);
        layoutMain[i]->addLayout(layoutOption[i]);

        tab[i]->setLayout(layoutMain[i]);
    }
    for (int j = 0; j < 20; j ++)
        ui->outPutPortComboBox->addItem(QString("COM%1").arg(j+1));
    for (int i = 0; i < ui->countChnSpinBox->value(); i ++)
        ui->channelTab->addTab(tab[i], QStringLiteral("通道%1").arg(i+1));
}

void MainWindow::initCOM()
{
    outputCOMSet = new PortSettings();
    for (int i = 0; i < ui->countChnSpinBox->value(); i ++)
    {
        inputCOMSet[i] = new PortSettings();
        if(QStringLiteral("固纬数字万用表") == dataSrcBox[i]->currentText())
        {
            inputCOMSet[i]->BaudRate = GDMINPUTRATE;    //115200
        }
        else
        {
            inputCOMSet[i]->BaudRate = SANHEINPUTRATE;  //4800
        }
        inputCOMSet[i]->Parity = PAR_NONE;
        inputCOMSet[i]->DataBits = DATA_8;
        inputCOMSet[i]->StopBits = STOP_1;
        inputCOMSet[i]->FlowControl = FLOW_OFF;
        inputCOMSet[i]->Timeout_Millisec = 500;

    }
    outputCOMSet->BaudRate = OUTPUTRATE;
    outputCOMSet->Parity = PAR_NONE;
    outputCOMSet->DataBits = DATA_8;
    outputCOMSet->StopBits = STOP_1;
    outputCOMSet->FlowControl = FLOW_OFF;
    outputCOMSet->Timeout_Millisec = 500;
}

void MainWindow::updateInputData(Win_QextSerialPort *dataCOM, QByteArray hexStr, int idx)
{
        qDebug() << "index: " << idx << " hexStr: " << QByteArray::fromHex(hexStr).data();
        qDebug() << "index: " << idx << " hexStr: " << hexStr.data();

        if (!hexStr.isEmpty())
        {
            qDebug() << "!hexStr.isEmpty()";
            qDebug() << QString("valueFlag:%1, beginFlag:%2").arg(valueFlag).arg(beginFlag);

            qDebug() << QString("dataSrcBox[idx]->currentText():%1").arg(dataSrcBox[idx]->currentText());
            if(dataSrcBox[idx]->currentText() == QStringLiteral("三和数显指示表"))
            {
                qDebug() << "Hex str: " << hexStr.mid(0,2);
                if ("aa" == hexStr.mid(0,2) || "AA" == hexStr.mid(0,2))
                {
                    valueDisplay[idx]->setText(QString("%1").arg(dataValue));
                    index = 0;
                    dataValue = 0.0;
                } else {
                    dataValue += hexStr.mid(0, 2).toDouble() * qPow(10, -4 + 2*index);
                    ++ index;
                }
                data[idx].setNum(dataValue);
            }
            else if (dataSrcBox[idx]->currentText() == QStringLiteral("固纬数字万用表"))
            {
                qDebug() << QString("valueFlag:%1, beginFlag:%2").arg(valueFlag).arg(beginFlag);
                if (true == valueFlag && (qstrncmp(GDM_connect_done_response.data(), QByteArray::fromHex(hexStr).data(), qstrlen(GDM_connect_done_response.data())-1)))
                {
                    data[idx] = QByteArray::fromHex(hexStr).split(',')[0].data();
                    sendGDMData(dataCOM, GDM_get_data.data());
                    qDebug() << "data: " << idx << ":::: " << data[idx];
                }
                else
                {
                    if (false == beginFlag)
                    {
                        sendGDMData(dataCOM, GDM_connect_cmd1.data());
                        qDebug() << "GDM_connect_cmd1:" << GDM_connect_cmd1.data();
                    }
                    qDebug() << " gw hex Str : " << QByteArray::fromHex(hexStr).data() << " GDM_CONNECT_RESPONSE1: " << GDM_connect_response1.data();
                    if (!qstrncmp(GDM_connect_response1.data(), QByteArray::fromHex(hexStr).data(), qstrlen(GDM_connect_response1.data())-1))
                    {
                        beginFlag = true;
                        sendGDMData(dataCOM, GDM_connect_cmd2.data());
                        qDebug() << "GDM_connect_cmd2: " << GDM_connect_cmd2.data();
                    } else if (!qstrncmp(GDM_connect_response2.data(), QByteArray::fromHex(hexStr).data(), qstrlen(GDM_connect_response2.data())-1)) {
                        qDebug() << "GDM_connect_response2: " << GDM_connect_response2.data();
                        sendGDMData(dataCOM, GDM_connect_done.data());
                        qDebug() << "GDM_connect_done: " << GDM_connect_done.data();
                    } else if (!qstrncmp(GDM_connect_done_response.data(), QByteArray::fromHex(hexStr).data(), qstrlen(GDM_connect_done_response.data())-1)){
                        valueFlag = true;
                        sendGDMData(dataCOM, GDM_get_data.data());
                        qDebug() << "GDM_get_data: " << GDM_get_data.data();
                    } else if (!qstrncmp(GDM_switchto_dcv_done.data(), QByteArray::fromHex(hexStr).data(), qstrlen(GDM_switchto_dcv_done.data())-1)) {
                        valueFlag = true;
                        qDebug() << "GDM_get_data: " << GDM_get_data.data();
                        sendGDMData(dataCOM, GDM_get_data.data());
                    } else if (!qstrncmp(GDM_switchto_acv_done.data(), QByteArray::fromHex(hexStr).data(), qstrlen(GDM_switchto_acv_done.data())-1)) {
                        valueFlag = true;
                        sendGDMData(dataCOM, GDM_get_data.data());
                        qDebug() << "GDM_get_data: " << GDM_get_data.data();
                    } else {
                        sendGDMData(dataCOM, GDM_get_data.data());
                        qDebug() << "GDM_get_data: " << GDM_get_data.data();
                    }
                }
            }
            else
            {

            }
        }
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
    if(QFile("customConfig.ini").exists()) {
        config = new QSettings("customConfig.ini", QSettings::IniFormat);
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
    qDebug() << "data run";

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
        {
            inputDataCOM[i] = new Win_QextSerialPort(inputCOMBox[i]->currentText(),
                                                     *inputCOMSet[i], QextSerialBase::EventDriven);
            inputDataCOM[i]->open(QIODevice::ReadWrite);

            connect(inputDataCOM[i], SIGNAL(readyRead()), this, SLOT(readInputData()));

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
            qDebug() << "index " << i << " : " << "inputCOMBox[i]->currentText(): " << inputCOMBox[i]->currentText();
        }

        outputDataCOM = new Win_QextSerialPort(ui->outPutPortComboBox->currentText(),
                                               *outputCOMSet, QextSerialBase::EventDriven);
        outputDataCOM->open(QIODevice::ReadWrite);

        timer->start(500);

    } else {
        for (int i = 0; i < ui->countChnSpinBox->value(); i ++)
        {
            disconnect(inputDataCOM[i], SIGNAL(readyRead()), this, SLOT(readInputData()));

            inputDataCOM[i]->close();
            delete inputDataCOM[i];
        }
        timer->stop();

        outputDataCOM->close();
        delete outputDataCOM;
    }
}

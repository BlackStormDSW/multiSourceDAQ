#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
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
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    checkAll();
}

void MainWindow::readInputData()
{
    for (int i = 0; i < ui->countChnSpinBox->value(); i ++)
    {
        inputBuf[i] = inputDataCOM[i]->readAll();
        updateInputData(inputDataCOM[i], inputBuf[i].toHex());
    }
}

void MainWindow::sendGDMData(Win_QextSerialPort *GDMCOM, QByteArray hexStr)
{
    GDMCOM->write(hexStr,hexStr.length());
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
    layoutTabWidget();
    initCOM();
    on_resetConfigButton_clicked();
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
            inputCOMSet[i]->BaudRate = GDMINPUTRATE;
//            inputCOMSet[i]->BaudRate = BAUD115200;
        }
        else
        {
            inputCOMSet[i]->BaudRate = SANHEINPUTRATE;
//            inputCOMSet[i]->BaudRate = BAUD4800;
        }
        inputCOMSet[i]->Parity = PAR_NONE;
        inputCOMSet[i]->DataBits = DATA_8;
        inputCOMSet[i]->StopBits = STOP_1;
        inputCOMSet[i]->FlowControl = FLOW_OFF;
        inputCOMSet[i]->Timeout_Millisec = 500;

        inputDataCOM[i] = new Win_QextSerialPort(inputCOMBox[i]->currentText(),
                                                 *inputCOMSet[i], QextSerialBase::EventDriven);
        inputDataCOM[i]->open(QIODevice::ReadWrite);

        connect(inputDataCOM[i], SIGNAL(readyRead()), this, SLOT(readInputData()));
    }
    outputCOMSet->BaudRate = OUTPUTRATE;
    outputCOMSet->Parity = PAR_NONE;
    outputCOMSet->DataBits = DATA_8;
    outputCOMSet->StopBits = STOP_1;
    outputCOMSet->FlowControl = FLOW_OFF;
    outputCOMSet->Timeout_Millisec = 500;
    outputDataCOM = new Win_QextSerialPort(ui->outPutPortComboBox->currentText(),
                                           *outputCOMSet, QextSerialBase::EventDriven);
    outputDataCOM->open(QIODevice::ReadWrite);
    connect(outputDataCOM, SIGNAL(readyRead()), this, SLOT(sendOutputData()));
}

void MainWindow::updateInputData(Win_QextSerialPort *dataCOM, QByteArray hexStr)
{
    static bool valueFlag[CHANNELMAX] = {false}, beginFlag[CHANNELMAX] = {false};
    for (int i = 0; i < CHANNELMAX; i ++)
    {
        if (!hexStr.isEmpty())
        {
            if(dataSrcBox[i]->currentText() == QStringLiteral("三和数显指示表"))
            {
                if ("aa" == hexStr.mid(0,2) || "AA" == hexStr.mid(0,2))
                {
                    valueDisplay[i]->setText(QString("%1").arg(dataValue));
                    index = 0;
                    dataValue = 0.0;
                } else {
                    dataValue += hexStr.mid(0, 2).toDouble() * qPow(10, -4 + 2*index);
                    ++ index;
                }
            }
            else if (dataSrcBox[i]->currentText() == QStringLiteral("固纬数字万用表"))
            {
                if (true == valueFlag[i])
                {
                    data[i] = QByteArray::fromHex(hexStr);
                }
                else
                {
                    if (false == beginFlag[i])
                    {
                        sendGDMData(dataCOM, GDM_CONNECT_CMD1);
                    }
                        if (GDM_CONNECT_RESPONSE1 == hexStr)
                        {
                            beginFlag[i] = true;
                            sendGDMData(dataCOM, GDM_CONNECT_CMD2);
                        } else if (GDM_CONNECT_RESPONSE2 == hexStr) {
                            sendGDMData(dataCOM, GDM_CONNECT_DONE);
                        } else if (GDM_CONNECT_DONE_RESPONSE == hexStr){
                            valueFlag[i] = true;
                        }
                }
            }
            else
            {

            }
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

void MainWindow::checkAll()
{
    checkCOM();
}

void MainWindow::checkCOM()
{
    bool checkOk = true;
    for(int i = 1; i < ui->countChnSpinBox->value(); i ++)
    {
        for (int j = 0; j < i; j ++)
        {
            if (inputCOMBox[j]->currentText() == inputCOMBox[i]->currentText())
            {
                QMessageBox msgBox;
                msgBox.setText(QStringLiteral("串口有冲突,请检查"));
                msgBox.exec();
                qDebug() << inputCOMBox[j]->currentText();
                checkOk = false;
                break;
            }
        }
        if (false == checkOk)
            break;
    }

    if(true == checkOk)
    {
        qDebug() << "read begin";
        readInputData();
        qDebug() << "read end";
        sendOutputData();
        qDebug() << "write end";
    }
}

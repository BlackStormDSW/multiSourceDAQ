/*************************************************************************
**  All rights reserved by Yantai XTD test technology co., LTD.			**
**																		**
**                          Author: Dong Shengwei						**
**          				Date: 2015-03-07                            **
*************************************************************************/

#include "inputdata.h"
#include "dataprotocol.h"
#include <QtMath>
#include <QDebug>

InputData::InputData()
{
    initGDMData();
    dataTmp = 0.0;
    dataValue = 0.0;
    dataSHStd = 0;
    index = 0;
}

InputData::~InputData()
{
    disconnect(inputDataCOM, SIGNAL(readyRead()), this, SLOT(readInputData()));

    inputDataCOM->close();
    delete inputDataCOM;
}

//初始化发送接收的GDM数据
void InputData::initGDMData()
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

//读取输入端口的数据
void InputData::readInputData()
{
    inputBuf = inputDataCOM->readAll();
//    qDebug() << " data: " << inputBuf.toHex();
    updateInputData(inputDataCOM, inputBuf.toHex());
}

//向数字万用表发送指令
void InputData::sendGDMData(Win_QextSerialPort *GDMCOM, QByteArray hexStr)
{
//    qDebug() << "sendGDMData: " << hexStr.data();
    GDMCOM->write(hexStr, hexStr.length());
}

//输出三和数显指示表位移的单位是公制还是英制
int InputData::getStd()
{
    return dataSHStd;
}

//初始化输入端口的设置
void InputData::initInputCOMSet()
{
    inputCOMSet = new PortSettings();
    if(QStringLiteral("固纬数字万用表") == dataSrc)
    {
        inputCOMSet->BaudRate = GDMINPUTRATE;    //115200
    } else {
        inputCOMSet->BaudRate = SANHEINPUTRATE;  //4800
    }
    inputCOMSet->Parity = PAR_NONE;
    inputCOMSet->DataBits = DATA_8;
    inputCOMSet->StopBits = STOP_1;
    inputCOMSet->FlowControl = FLOW_OFF;
    inputCOMSet->Timeout_Millisec = INTERVAL;
}

//更新输入数据
void InputData::updateInputData(Win_QextSerialPort *dataCOM, QByteArray hexStr)
{
    if (!hexStr.isEmpty())
    {
//        qDebug() << QString("valueFlag:%1, beginFlag:%2").arg(valueFlag).arg(beginFlag);

        if(QStringLiteral("三和数显指示表") == dataSrc)
        {
//            qDebug() << "Hex str: " << hexStr.mid(0,2);
            if ("aa" == hexStr.mid(0,2) || "AA" == hexStr.mid(0,2))
            {
                index = 0;
                dataTmp = 0.0;
            } else {
                if (3 > index)
                {
                    dataTmp += hexStr.mid(0, 2).toDouble() * qPow(10, -4 + 2*index);
                } else if (3 == index) {
                    dataSHStd = hexStr.mid(0,2).toInt()%2;
                    if (1 == dataSHStd)
                        dataTmp = dataTmp/10.0;
                    if((hexStr.mid(0,2).toInt()/2)%2)
                        dataValue = -dataTmp;
                    else
                        dataValue = dataTmp;
                }
                ++ index;
            }
//            qDebug() << "updateInputData data value : " << dataValue;
        }
        else //(QStringLiteral("固纬数字万用表") == dataSrc)
        {
            if (true == valueFlag && (qstrncmp(GDM_connect_done_response.data(), QByteArray::fromHex(hexStr).data(), qstrlen(GDM_connect_done_response.data())-1)))
            {
                dataValue = QByteArray::fromHex(hexStr).split(',')[0].toDouble();
                sendGDMData(dataCOM, GDM_get_data.data());
            }
            else
            {
                if (false == beginFlag)
                {
                    sendGDMData(dataCOM, GDM_connect_cmd1.data());
//                    qDebug() << "GDM_connect_cmd1:" << GDM_connect_cmd1.data();
                }
                if (!qstrncmp(GDM_connect_response1.data(), QByteArray::fromHex(hexStr).data(), qstrlen(GDM_connect_response1.data())-1))
                {
                    beginFlag = true;
                    sendGDMData(dataCOM, GDM_connect_cmd2.data());
//                    qDebug() << "GDM_connect_cmd2: " << GDM_connect_cmd2.data();
                } else if (!qstrncmp(GDM_connect_response2.data(), QByteArray::fromHex(hexStr).data(), qstrlen(GDM_connect_response2.data())-1)) {
//                    qDebug() << "GDM_connect_response2: " << GDM_connect_response2.data();
                    sendGDMData(dataCOM, GDM_connect_done.data());
                } else if (!qstrncmp(GDM_connect_done_response.data(), QByteArray::fromHex(hexStr).data(), qstrlen(GDM_connect_done_response.data())-1)){
                    valueFlag = true;
//                    qDebug() << "GDM_get_data: " << GDM_get_data.data();
                    if (!adcValue.compare(QString("DCV")))
                        sendGDMData(dataCOM, GDM_switchto_dcv.data());
                    else if (!adcValue.compare(QString("ACV")))
                        sendGDMData(dataCOM, GDM_switchto_acv.data());
                    sendGDMData(dataCOM, GDM_get_data.data());
                } else if (!qstrncmp(GDM_switchto_dcv_done.data(), QByteArray::fromHex(hexStr).data(), qstrlen(GDM_switchto_dcv_done.data())-1)) {
                    valueFlag = true;
//                    qDebug() << "GDM_get_data: " << GDM_get_data.data();
                    sendGDMData(dataCOM, GDM_get_data.data());
                } else if (!qstrncmp(GDM_switchto_acv_done.data(), QByteArray::fromHex(hexStr).data(), qstrlen(GDM_switchto_acv_done.data())-1)) {
                    valueFlag = true;
//                    qDebug() << "GDM_get_data: " << GDM_get_data.data();
                    sendGDMData(dataCOM, GDM_get_data.data());
                } else {
//                    qDebug() << "GDM_get_data: " << GDM_get_data.data();
                    sendGDMData(dataCOM, GDM_get_data.data());
                }
            }
        }
    }
}

//设置数字输入端口名称
void InputData::setCOMName(QString COMName)
{
    inputCOMName = COMName;
    //当端口号大于9，则需要在端口名之前添加字符串"\\\\.\\"，否则端口打不开
    if(inputCOMName.size()>4)
    {
        inputCOMName.insert(0,"\\\\.\\");
    }
}

//设置数据来源名称
void InputData::setDataSrc(QString src)
{
    dataSrc = src;
}

//输出数据
double InputData::getData()
{
    return dataValue;
}

//初始化输入端口
void InputData::initInputCOM()
{
    valueFlag = false;
    beginFlag = false;
    adcValue = QString("DCV");
    initInputCOMSet();
    inputDataCOM = new Win_QextSerialPort(inputCOMName,
                                          *inputCOMSet, QextSerialBase::EventDriven);
    inputDataCOM->open(QIODevice::ReadWrite);

    setInputCOM();

    connect(inputDataCOM, SIGNAL(readyRead()), this, SLOT(readInputData()));
}

//配置输入端口设置
void InputData::setInputCOM()
{
    if(QStringLiteral("固纬数字万用表") == dataSrc)
    {
        inputDataCOM->setBaudRate(GDMINPUTRATE);    //115200
    } else {
        inputDataCOM->setBaudRate(SANHEINPUTRATE);  //4800
    }

    inputDataCOM->setDataBits(DATA_8);
    inputDataCOM->setParity(PAR_NONE);
    inputDataCOM->setStopBits(STOP_1);
    inputDataCOM->setFlowControl(FLOW_OFF);
    inputDataCOM->setTimeout(INTERVAL);
}

//关闭输入端口
void InputData::closeInputCOM()
{
    disconnect(inputDataCOM, SIGNAL(readyRead()), this, SLOT(readInputData()));
    inputDataCOM->close();
    delete inputDataCOM;
}

//向数字万用表发送链接指令，开始读取数字万用表数据
void InputData::runGDM(QString adc)
{
    adcValue = adc;
    sendGDMData(inputDataCOM, GDM_connect_cmd1);
}

//切换数字万用表的交流/直流
void InputData::changADC(QString adc)
{
    qDebug() << "adc: " << adc;
    if (!adc.compare("ACV"))
    {
        qDebug() << "ACV";
        sendGDMData(inputDataCOM, GDM_switchto_acv.data());
    } else if (!adc.compare("DCV")) {
        qDebug() << "DCV";
        sendGDMData(inputDataCOM, GDM_switchto_dcv.data());
    }
}

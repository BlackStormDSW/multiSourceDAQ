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

void InputData::readInputData()
{
    inputBuf = inputDataCOM->readAll();
//    qDebug() << " data: " << inputBuf.data();
    qDebug() << " data: " << inputBuf.toHex();
//    qDebug() << " data: " << QByteArray::fromHex(inputBuf.data());
    updateInputData(inputDataCOM, inputBuf.toHex());
}

void InputData::sendGDMData(Win_QextSerialPort *GDMCOM, QByteArray hexStr)
{
    qDebug() << "sendGDMData: " << hexStr.data();
    GDMCOM->write(hexStr, hexStr.length());
}

void InputData::initInputCOM()
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
    inputCOMSet->Timeout_Millisec = 500;
}

void InputData::updateInputData(Win_QextSerialPort *dataCOM, QByteArray hexStr)
{

    if (!hexStr.isEmpty())
    {
        qDebug() << QString("valueFlag:%1, beginFlag:%2").arg(valueFlag).arg(beginFlag);

        if(QStringLiteral("三和数显指示表") == dataSrc)
        {
            qDebug() << "Hex str: " << hexStr.mid(0,2);
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
                    if((hexStr.mid(0,2).toInt()/2)%2)
                        dataValue = -dataTmp;
                    else
                        dataValue = dataTmp;
                }
                ++ index;
            }
            qDebug() << "updateInputData data value : " << dataValue;
        }
        else //(QStringLiteral("固纬数字万用表") == dataSrc)
        {
            qDebug() << QString("valueFlag:%1, beginFlag:%2").arg(valueFlag).arg(beginFlag);
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
    }
}

void InputData::setCOMName(QString COMName)
{
    inputCOMName = COMName;
}

void InputData::setDataSrc(QString src)
{
    dataSrc = src;
}

double InputData::getData()
{
    qDebug() << "data value is : " << dataValue;
    return dataValue;
}

void InputData::init()
{
    valueFlag = false;
    beginFlag = false;
    initInputCOM();
    inputDataCOM = new Win_QextSerialPort(inputCOMName,
                                          *inputCOMSet, QextSerialBase::EventDriven);
    inputDataCOM->open(QIODevice::ReadWrite);
    connect(inputDataCOM, SIGNAL(readyRead()), this, SLOT(readInputData()));
}

void InputData::run()
{
    sendGDMData(inputDataCOM, GDM_connect_cmd1);
}

/*************************************************************************
**  All rights reserved by Yantai XTD test technology co., LTD.			**
**																		**
**                          Author: Dong Shengwei						**
**          				Date: 2015-03-07                            **
*************************************************************************/

#include "outputdata.h"
#include "dataprotocol.h"
#include <QDebug>

OutputData::OutputData()
{
    timer = new QTimer;
}

OutputData::~OutputData()
{
    outputDataCOM->close();
    delete outputDataCOM;
}

//初始化输出端口设置
void OutputData::initOutputCOMSet()
{
    outputCOMSet = new PortSettings();
    outputCOMSet->BaudRate = OUTPUTRATE;
    outputCOMSet->Parity = PAR_NONE;
    outputCOMSet->DataBits = DATA_8;
    outputCOMSet->StopBits = STOP_1;
    outputCOMSet->FlowControl = FLOW_OFF;
    outputCOMSet->Timeout_Millisec = INTERVAL;
}

//配置输出端口设置
void OutputData::setOutputCOM()
{
    outputDataCOM->setBaudRate(OUTPUTRATE);
    outputDataCOM->setDataBits(DATA_8);
    outputDataCOM->setParity(PAR_NONE);
    outputDataCOM->setStopBits(STOP_1);
    outputDataCOM->setFlowControl(FLOW_OFF);
    outputDataCOM->setTimeout(INTERVAL);
}

//初始化串口
void OutputData::initOutputCOM()
{
    initOutputCOMSet();
    outputDataCOM = new Win_QextSerialPort(outputCOMName,
                                           *outputCOMSet, QextSerialBase::EventDriven);
    outputDataCOM->open(QIODevice::ReadWrite);

    setOutputCOM();

    connect(timer, SIGNAL(timeout()), this, SLOT(sendOutputData()));
    timer->start(INTERVAL);
}

//关闭输出端口
void OutputData::closeOutputCOM()
{
    timer->stop();
    outputDataCOM->close();
    delete outputDataCOM;
}

//向输出端口发送数据
void OutputData::sendOutputData()
{
    outputDataCOM->write(data, data.length());
}

//设置输出端口的名称
void OutputData::setCOMName(QString COMName)
{
    outputCOMName = COMName;
}

//获取要输出的数据
void OutputData::setData(QByteArray dataIn)
{
    data = dataIn;
}


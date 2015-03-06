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

void OutputData::initOutputCOM()
{
    qDebug () << "outputCOMName: " << outputCOMName;
    outputCOMSet = new PortSettings();
    outputCOMSet->BaudRate = OUTPUTRATE;
    outputCOMSet->Parity = PAR_NONE;
    outputCOMSet->DataBits = DATA_8;
    outputCOMSet->StopBits = STOP_1;
    outputCOMSet->FlowControl = FLOW_OFF;
    outputCOMSet->Timeout_Millisec = 500;

    outputDataCOM = new Win_QextSerialPort(outputCOMName,
                                           *outputCOMSet, QextSerialBase::EventDriven);
    outputDataCOM->open(QIODevice::ReadWrite);

    connect(timer, SIGNAL(timeout()), this, SLOT(sendOutputData()));
    timer->start(500);
}

void OutputData::sendOutputData()
{
    qDebug() << "OOOOOOOOOOOOOOOOOOOOOO: " << data;
    outputDataCOM->write(data, data.length());
}

void OutputData::setCOMName(QString COMName)
{
    outputCOMName = COMName;
}

void OutputData::setData(QByteArray dataIn)
{
    data = dataIn;
}


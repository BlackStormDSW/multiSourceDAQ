/*************************************************************************
**  All rights reserved by Yantai XTD test technology co., LTD.			**
**																		**
**          				Author: Dong Shengwei						**
**          				Date: 2015-03-07							**
*************************************************************************/

#ifndef OUTPUTDATA_H
#define OUTPUTDATA_H

#include <QObject>
#include <QTimer>
#include "win_qextserialport.h"

class OutputData : public QObject
{
    Q_OBJECT
public:
    explicit OutputData();
    ~OutputData();
private:
    PortSettings *outputCOMSet;
    Win_QextSerialPort *outputDataCOM;

    QString outputCOMName;
    QByteArray data;

    QTimer *timer;

    bool valueFlag, beginFlag;

public:
    //初始化串口
    void initOutputCOM();
    void setCOMName(QString COMName);
    void setData(QByteArray dataIn);
private slots:
    void sendOutputData();
};


#endif // OUTPUTDATA_H

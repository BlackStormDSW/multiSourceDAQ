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
    //输出端口的设置
    PortSettings *outputCOMSet;
    //输出端口
    Win_QextSerialPort *outputDataCOM;
    //输出端口的名称
    QString outputCOMName;
    //向输出端口发送的数据
    QByteArray data;
    //定时器
    QTimer *timer;

public:
    //初始化输出端口
    void initOutputCOM();

    //关闭输出端口
    void closeOutputCOM();

    //设置输出端口的名称
    void setCOMName(QString COMName);

    //获取要输出的数据
    void setData(QByteArray dataIn);
private slots:
    //向输出端口发送数据
    void sendOutputData();
};


#endif // OUTPUTDATA_H

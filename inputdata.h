/*************************************************************************
**  All rights reserved by Yantai XTD test technology co., LTD.			**
**																		**
**          				Author: Dong Shengwei						**
**          				Date: 2015-03-07							**
*************************************************************************/

#ifndef INPUTDATA_H
#define INPUTDATA_H

#include <QObject>
#include "win_qextserialport.h"

class InputData : public QObject
{
    Q_OBJECT
public:
    explicit InputData();
    ~InputData();
private:
    //输入端口
    Win_QextSerialPort *inputDataCOM;
    //输入的数据
    QByteArray inputBuf;

    //向数字万用表发送的指令，接收数字万用表的响应
    QByteArray GDM_connect_cmd1, GDM_connect_cmd2,
    GDM_connect_response1, GDM_connect_response2,
    GDM_connect_done, GDM_connect_done_response,
    GDM_switchto_dcv, GDM_switchto_dcv_done,
    GDM_switchto_acv, GDM_switchto_acv_done, GDM_get_data;

    //中间变量
    int index;
    double dataTmp;
    //读取到的数字
    double dataValue;

    int dataSHStd;    //0:公制 1:英制
    //输入端口名称，数据来源名称
    QString inputCOMName, dataSrc;
    //输入端口的设置
    PortSettings *inputCOMSet;
    //中间变量
    bool valueFlag, beginFlag;
    //交直流变量
    QString adcValue;

private:
    //初始化输入端口的设置
    void initInputCOMSet();

    //更新输入数据
    void updateInputData(Win_QextSerialPort *dataCOM, QByteArray hexStr);

public:
    //设置数字输入端口名称
    void setCOMName(QString COMName);

    //设置数据来源名称
    void setDataSrc(QString src);

    //输出数据
    double getData();

    //初始化输入端口
    void initInputCOM();

    //关闭输入端口
    void closeInputCOM();

    //向数字万用表发送链接指令，开始读取数字万用表数据
    void runGDM(QString adc);

    //初始化发送接收的GDM数据
    void initGDMData();

    //向数字万用表发送指令
    void sendGDMData(Win_QextSerialPort *GDMCOM, QByteArray hexStr);

    //输出三和数显指示表位移的单位是公制还是英制
    int getStd();

public slots:
    //读取输入端口的数据
    void readInputData();

    //切换数字万用表的交流/直流
    void changADC(QString adc);
};

#endif // INPUTDATA_H

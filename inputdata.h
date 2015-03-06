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
    Win_QextSerialPort *inputDataCOM;
    QByteArray inputBuf;

    QByteArray GDM_connect_cmd1, GDM_connect_cmd2,
    GDM_connect_response1, GDM_connect_response2,
    GDM_connect_done, GDM_connect_done_response,
    GDM_switchto_dcv, GDM_switchto_dcv_done,
    GDM_switchto_acv, GDM_switchto_acv_done, GDM_get_data;

    int index;
    double dataValue, dataTmp;

    int dataSHStd;    //0:公制 1:英制

    QString inputCOMName, dataSrc;

    PortSettings *inputCOMSet;

public:
    bool valueFlag, beginFlag;

private:
    //初始化串口
    void initInputCOM();

    void updateInputData(Win_QextSerialPort *dataCOM, QByteArray hexStr);

public:
    void setCOMName(QString COMName);
    void setDataSrc(QString src);
    double getData();
    void init();
    void run();

    //初始化发送接收的GDM数据
    void initGDMData();

    void sendGDMData(Win_QextSerialPort *GDMCOM, QByteArray hexStr);

public slots:
    void readInputData();
};

#endif // INPUTDATA_H

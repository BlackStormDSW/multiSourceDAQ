#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dataprotocol.h"
#include "readdata.h"
#include "senddata.h"
#include "inputdata.h"
#include "outputdata.h"

namespace Ui {
class MainWindow;
}

class QSettings;
class QLabel;
class QLayout;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QComboBox;
class QLineEdit;
class QSpacerItem;
class QTextCodec;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_saveCfgButton_clicked();

    void on_resetConfigButton_clicked();

    void on_countChnSpinBox_valueChanged(int);

    void on_startButton_clicked();

    void handleData();

private:
    //初始化
    void init();

    //选项卡的布局
    void layoutTabWidget();

    void display();

    //初始化配置
    void initConfig();

    //运行接收数据与发送数据
    void dataRun();

private:
    Ui::MainWindow *ui;
    /*defaultConfig:默认配置；customConfig：自定义配置*/
    QSettings *defaultConfig, *customConfig, *config;
    /*
    channelCount            ：当前通道数目；
    outputCOM               ：输出端口号(所有输入端口汇入同一个输出端口);
    dataSrc[CHANNELMAX]     ：数据来源；
    inputCOM[CHANNELMAX]    ：输入端口号；
    ADCSwitch[CHANNELMAX]   ：交流/直流开关
    */

    QLabel *dataSrcLabel[CHANNELMAX], *inputCOMLabel[CHANNELMAX], *valueLabel[CHANNELMAX],
    *ADCSwitchLabel[CHANNELMAX], *unitLabel[CHANNELMAX];
    QComboBox *dataSrcBox[CHANNELMAX], *inputCOMBox[CHANNELMAX], *ADCSwitchBox[CHANNELMAX];
    QLineEdit *valueDisplay[CHANNELMAX];
    QWidget *tab[CHANNELMAX];

    QHBoxLayout *layoutInputCOM[CHANNELMAX], *layoutValue[CHANNELMAX],
    *layoutADC[CHANNELMAX], *layoutMain[CHANNELMAX];
    QVBoxLayout *layoutLabel[CHANNELMAX], *layoutOption[CHANNELMAX];
    QGridLayout *layoutTab[CHANNELMAX];

    QSpacerItem *spacer1[CHANNELMAX], *spacer2[CHANNELMAX];

    InputData *inData[CHANNELMAX];

    OutputData *outData;

    QByteArray data[CHANNELMAX];

    QString dataStr;

    //接收与发送数据是否开始的标志
    bool runFlag;

    //发送数据的定时器
    QTimer *timer;

    QTextCodec *codec;
};

#endif // MAINWINDOW_H

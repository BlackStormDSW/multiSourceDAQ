/*************************************************************************
**  All rights reserved by Yantai XTD test technology co., LTD.			**
**																		**
**          				Author: Dong Shengwei						**
**          				Date: 2015-03-07							**
*************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dataprotocol.h"
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
    //保存配置(将用户当前配置保存到自定义配置文件中)
    void on_saveCfgButton_clicked();

    //重置配置(从出厂配置文件中读取初始化数值)
    void on_resetConfigButton_clicked();

    //通道数目改变时，触发窗口显示改变
    void on_countChnSpinBox_valueChanged(int);

    //点击开始按钮，开始或停止数据采集
    void on_startButton_clicked();

    //将输入通道的数据组合
    void handleData();

    //在标签页中显示当前通道的参数，如数据名称、单位等
    void showInterface();

private:
    //初始化
    void init();

    //选项卡的布局
    void layoutTabWidget();

    //界面显示
    void display();

    //初始化配置
    void initConfig();

    //开始或停止运行
    void dataRun();

    //在标签页中显示指定通道的参数，如数据名称、单位以及实时数据等
    void showData(int i);

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
    QLabel *nullLabel1[CHANNELMAX], *nullLabel2[CHANNELMAX];
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

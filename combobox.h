/*************************************************************************
** 	All rights reserved by Yantai XTD test technology co., LTD.			**
**																		**
**          				Author: DongShengwei						**
**          				Date: 2015-3-14								**
*************************************************************************/

#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QStringList>
#include <QSettings>
#include <QString>
#include <Windows.h>

class ComboBox : public QComboBox
{
public:
    ComboBox();
    ~ComboBox();

private:
    //获取可用串口的名称
    QString getCOMName(int idx, QString para);

    bool event(QEvent *event);

public:
    //更新串口列表
    void updateCOMList();

private:
    //注册表中串口路径
    QString serialPath;

    //串口设置
    QSettings *serialSettings;

    //串口键值链表
    QStringList serialKeyList;

    //串口名称链表
    QStringList comNameList;

    QStringList key;

    wchar_t subKey[80];     //子键
    wchar_t keyName[256];   //键名数组
    wchar_t keyValue[256];  //键值数组
    int indexNum;
    DWORD keySize, valueSize, type;
    HKEY hKey;

};

#endif // COMBOBOX_H

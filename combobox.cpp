/*************************************************************************
** 	All rights reserved by Yantai XTD test technology co., LTD.			**
**																		**
**          				Author: DongShengwei						**
**          				Date: 2015-3-14								**
*************************************************************************/

#include "combobox.h"
#include "WinReg.h"
#include <QMouseEvent>
#include <QStringListIterator>
#include <QDebug>

ComboBox::ComboBox()
{
    serialPath = "HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM";
    serialSettings = new QSettings(serialPath, QSettings::NativeFormat);
}

ComboBox::~ComboBox()
{

}

//获取可用串口的名称
QString ComboBox::getCOMName(int idx, QString para)
{
    QString comName = "";       //串口名称
    QString strKey = "HARDWARE\\DEVICEMAP\\SERIALCOMM"; //子键路径
    int a = strKey.toWCharArray(subKey);
    subKey[a] = L'\0';
    if(0 != ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ|KEY_QUERY_VALUE, &hKey))
    {
        QString error = "Cannot open regedit!";
        qDebug() << error;
    }

    QString keyMessage = "";    //键名
    QString message = "";
    QString valueMesage = "";   //键值
    indexNum = idx;             //要读取键值的索引号

    keySize = sizeof(keyName);
    valueSize = sizeof(keyValue);

    if(0 == ::RegEnumValue(hKey, indexNum, keyName, &keySize, 0, &type, (BYTE *)keyValue, &valueSize))
    {
        //读取键名
        for (int i = 0; i < keySize; i ++)
        {
            message = QString::fromStdWString(keyName);
            keyMessage.append(message);
        }
        //读取键值
        for (int j = 0; j < valueSize; j ++)
        {
            if (0x00 != keyValue[j])
            {
                valueMesage.append(keyValue[j]);
            }
        }

        if ("key" == para)
        {
            comName = keyMessage;
        } else if ("value" == para) {
            comName = valueMesage;
            if (keyMessage.contains("VSerial"))
            {
                comName.append("(Virtual)");
            }
        } else {
            comName = "noKey";
        }
    } else {
        comName = "noKey";
    }
    RegCloseKey(hKey);
    return comName;
}

void ComboBox::updateCOMList()
{
    this->clear();
    key = serialSettings->allKeys();
    for (int i = 0; i < key.size(); i ++)
    {
        this->addItem(getCOMName(i, "value"));
    }
}

bool ComboBox::event(QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        updateCOMList();
        this->showPopup();
        return true;
    } else {
        return QComboBox::event(event);
    }
}

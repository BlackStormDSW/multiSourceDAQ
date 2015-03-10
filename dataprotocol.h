/*************************************************************************
**  All rights reserved by Yantai XTD test technology co., LTD.			**
**																		**
**          				Author: Dong Shengwei						**
**          				Date: 2015-02-01							**
**          				File: dataprotocol.h						**
*************************************************************************/

#ifndef DATAPROTOCOL_H
#define DATAPROTOCOL_H

//最大通道数
#define CHANNELMAX 8
//数字万用表端口波特率
#define GDMINPUTRATE BAUD115200
//三和数字显示表端口波特率
#define SANHEINPUTRATE BAUD4800
//输出端口(东华3815N静态应变仪侧)波特率
#define OUTPUTRATE BAUD9600

//输出端口发送数据的间隔(ms)
#define INTERVAL 500

/*向数字万用表发送的指令，接收数字万用表的响应*/
#define GDM_CONNECT_CMD1 "2A 53 52 45 3F 0A"
#define GDM_CONNECT_RESPONSE1 "30 30 30 0D"
#define GDM_CONNECT_CMD2 "53 59 53 54 3A 4F 55 54 50 3A 45 4F 46 20 32 0A 53 59 53 54 3A 50 41 52 3A 53 41 56 45 20 35 0A 53 59 53 54 3A 50 41 52 3A 4C 4F 41 44 20 35 0A 2A 49 44 4E 3F 0A"
#define GDM_CONNECT_RESPONSE2 "47 57 49 6E 73 74 65 6B 2C 47 44 4D 38 32 36 31 41 2C 47 45 4F 31 36 30 35 35 38 2C 31 2E 30 32 0D"
#define GDM_CONNECT_DONE "73 65 6E 73 3A 64 65 74 3A 72 61 74 65 3F 0A"
#define GDM_CONNECT_DONE_RESPONSE "53 4C 4F 57 0D"
#define GDM_SWITCHTO_DCV "73 79 73 74 3A 6D 61 74 68 3A 63 68 61 6E 20 30 0A 63 61 6C 63 3A 73 74 61 74 20 30 0A 63 6F 6E 66 3A 76 6F 6C 74 3A 64 63 0A 73 65 6E 73 3A 64 65 74 3A 72 61 74 65 3F 0A"
#define GDM_SWITCHTO_DCV_DONE "53 4C 4F 57 0D"
#define GDM_SWITCHTO_ACV "73 79 73 74 3A 6D 61 74 68 3A 63 68 61 6E 20 30 0A 63 61 6C 63 3A 73 74 61 74 20 30 0A 63 6F 6E 66 3A 76 6F 6C 74 3A 61 63 0A 73 65 6E 73 3A 64 65 74 3A 62 61 6E 64 3F 0A"
#define GDM_SWITCHTO_ACV_DONE "2B 30 30 33 0D"
#define GDM_GET_DATA "72 65 61 64 3F 20 62 75 66 0A"

#endif // DATAPROTOCOL_H


#ifndef MULTISRCDAQ_H
#define MULTISRCDAQ_H

#include <QObject>

class multiSrcDAQ : public QObject
{
    Q_OBJECT
public:
    explicit multiSrcDAQ(QObject *parent = 0);
    ~multiSrcDAQ();

signals:

public slots:
};

#endif // MULTISRCDAQ_H

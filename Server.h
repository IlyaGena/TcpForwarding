#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QString>

class Server : public QObject
{
    Q_OBJECT
public:
    Server(QString address);

private:
    QString mm_ipAaddr;
    QString mm_port;
};

#endif // SERVER_H

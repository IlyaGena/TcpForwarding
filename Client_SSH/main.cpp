#include <QCoreApplication>

#include "Client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString arg1 = argv[1];
    QString arg2 = argv[2];
    QString arg3 = argv[3];

    if (arg1 == "client" && !arg2.isEmpty() && !arg3.isEmpty())
    {
        Client client(arg2, arg3);
        return a.exec();
    }
    else
    {
        qDebug() << "Несовпадение по формату: 'Client_SSH client <remote_ip>:<remote_port:><local_ip>:<local_port> <server_api_ip>:<server_api_port>'";
        exit(EXIT_FAILURE);
    }
}

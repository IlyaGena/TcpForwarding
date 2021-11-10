#include <QCoreApplication>
#include "Server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString arg1 = argv[1];
    QString arg2 = argv[2];

    if (arg1 == "server")
    {
        Server server(arg2);
        return a.exec();
    }
    else
    {
        qDebug() << "Несовпадение по формату: 'Server_SSH server <server_api_ip>:<server_api_port>'";
        exit(EXIT_FAILURE);
    }

}

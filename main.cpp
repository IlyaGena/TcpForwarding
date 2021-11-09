#include <QCoreApplication>

#include "Server.h"

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "Russian");
    QCoreApplication a(argc, argv);

    QString arg1 = argv[1];
    QString arg2 = argv[2];

    if (arg1 == "server")
        Server server(arg2);

    return a.exec();
}

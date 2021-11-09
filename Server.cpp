#include "Server.h"

Server::Server(QString address)
{
    qDebug() << "Вы запустили сервер";
    QStringList listGeneralAddr = address.split(":");
    QStringList listAddr = listGeneralAddr[0].split(".");

    foreach (auto record, listAddr)
    {
        auto recordInt = record.toInt();
        if  (recordInt < 0 || recordInt > 255)
        {
            qDebug() << "Адрес вышел за пределы допустимых значений";
            exit(EXIT_FAILURE);
        }
    }

    if (listGeneralAddr[1].toInt() < 0 || listGeneralAddr[1].toInt() > 65535)
    {
        qDebug() << "Порт вышел за пределы допустимых значений";
        exit(EXIT_FAILURE);
    }

    if (listGeneralAddr.count() != 2 ||
            listAddr.count() != 4)
    {
        qDebug() << "Адрес не соответствует шаблону adress:port";
        exit(EXIT_FAILURE);
    }

    mm_ipAaddr = listGeneralAddr[0];
    mm_port = listGeneralAddr[1];

    qDebug() << "Ваши аргументы:";
    qDebug() << "IP: " << mm_ipAaddr;
    qDebug() << "Port: " << mm_port;
}

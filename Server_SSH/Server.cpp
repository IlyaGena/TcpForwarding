#include "Server.h"

inline bool checkAddres(QStringList listGeneralAddr, QStringList listAddr)
{
    foreach (auto record, listAddr)
    {
        auto recordInt = record.toInt();
        if  (recordInt < 0 || recordInt > 255)
        {
            qDebug() << "Адрес вышел за пределы допустимых значений";
            return false;
        }
    }

    if (listGeneralAddr.count() != 2 ||
            listAddr.count() != 4)
    {
        qDebug() << "Адрес не соответствует шаблону adress:port";
        return false;
    }
    return true;
}

inline bool checkPort(QStringList listGeneralAddr)
{
    if (listGeneralAddr[1].toInt() < 0 || listGeneralAddr[1].toInt() > 65535)
    {
        qDebug() << "Порт вышел за пределы допустимых значений";
        return false;
    }
    return true;
}

Server::Server(QString address) :
    mm_server(),
    mm_ipAddrServer(),
    mm_portServer(),
    mm_timer()
{
    qDebug() << "Вы запустили сервер";

    // парсинг введеных значений
    parsingAddressServer(address);

    // запуск сервера
    mm_server.listen(mm_ipAddrServer, mm_portServer);

    // соединение сигнал-слот
    connect(&mm_server, &QTcpServer::newConnection,
            this, &Server::newConnectionToServer, Qt::QueuedConnection);

    // таймер проверки клиентов
    mm_timer.setInterval(TIMERCOUNT);
    connect(&mm_timer, &QTimer::timeout,
            this, &Server::keppALive, Qt::QueuedConnection);
    mm_timer.start();
}

Server::~Server()
{
    mm_server.close();
}

void Server::newConnectionToServer()
{
    QTcpServer *server = static_cast<QTcpServer*>(sender());

    QTcpSocket *client = server->nextPendingConnection();

    connect(client, &QTcpSocket::readyRead,
            this, &Server::binaryMessageReceivedServer);

    connect(client, &QTcpSocket::disconnected,
            this, &Server::disconnected);

    QString message = QString("<Tunnel> соединение открыто %1:%2").arg(
                QHostAddress(client->peerAddress().toIPv4Address()).toString(),
                QString::number(client->peerPort()));
    qDebug() << message;

    // добавление указателя на сокет в список контроля keepAlive
    quint64 nowTime = QDateTime::currentSecsSinceEpoch();
    mm_connectedList.insert(client, nowTime);

    mm_forwardList.insert(client, nullptr);
}

void Server::disconnected()
{
    QTcpSocket *client = static_cast<QTcpSocket*>(sender());

    QString message = QString("<Tunnel> соединение закрыто %1:%2").arg(
                QHostAddress(client->peerAddress().toIPv4Address()).toString(),
                QString::number(client->peerPort()));
    qDebug() << message;

    // удаление сокета из списка контроля keepAlive
    mm_connectedList.remove(client);

    if (mm_forwardList.contains(client))
    {
        auto remoteSrv = mm_forwardList[client];
        remoteSrv->getServer()->close();
        mm_forwardList.remove(client);
    }
}

void Server::binaryMessageReceivedServer()
{
    QTcpSocket *client = static_cast<QTcpSocket*>(sender());
    QByteArray data = client->readAll();

    qDebug() << "<Tunnel> Принял: |" << data << "|";

    // изменяем время последнего сообщения
    auto list = mm_connectedList.keys();
    foreach (auto record, list)
    {
        if (record == client)
        {
            quint64 nowTime = QDateTime::currentSecsSinceEpoch();
            mm_connectedList[record] = nowTime;
            break;
        }
    }

    foreach (auto record, mm_forwardList.keys())
    {
        if (record != client)
            continue;

        // генерация нового сервера remote
        if (mm_forwardList[record] == nullptr)
        {
            QHostAddress ipAddrRemote;
            quint16 portRemote;

            if (!parsingAddressRemote(data, ipAddrRemote, portRemote))
                return;

            RemoteServer* remoteServer = new RemoteServer(ipAddrRemote, portRemote);

            connect(remoteServer, &RemoteServer::sendDataFromRemote,
                    this, &Server::sendDataFromRemote);

            mm_forwardList.insert(client, remoteServer);
            return;
        }
        // отправка ответа сокету сервера remote
        else if (mm_forwardList[record] != nullptr)
        {
            RemoteServer* server = mm_forwardList.value(record);
            server->sendDataFromServer(data);
        }
    }
}

void Server::sendDataFromRemote(QByteArray data)
{
    RemoteServer *remoteServer = static_cast<RemoteServer*>(sender());
    foreach(auto record, mm_forwardList.keys())
    {
        if(mm_forwardList.value(record) == remoteServer)
        {
            record->write(data);
            record->flush();
        }
    }
}

void Server::keppALive()
{
    if(mm_connectedList.isEmpty())
        return;

    quint64 nowTime = QDateTime::currentSecsSinceEpoch();

    // проверка последнего подключения
    auto list = mm_connectedList.keys();
    foreach (auto record, list)
    {
        auto deffTime = nowTime - mm_connectedList.value(record);
        if(deffTime > KEEPALIVE)
        {
            record->close();
            mm_connectedList.remove(record);

            if (mm_forwardList.contains(record))
            {
                auto remoteSrv = mm_forwardList[record];
                remoteSrv->getServer()->close();
                mm_forwardList.remove(record);
            }
        }
    }
}

void Server::parsingAddressServer(QString address)
{
    qDebug() << "<Tunnel> Начат процесс определения адреса прослушивания...";
    QStringList listGeneralAddr = address.split(":");
    QStringList listAddr = listGeneralAddr[0].split(".");

    if (!checkAddres(listGeneralAddr, listAddr) || !checkPort(listGeneralAddr))
        exit(EXIT_FAILURE);

    mm_ipAddrServer = QHostAddress(listGeneralAddr[0]);
    mm_portServer = listGeneralAddr[1].toUInt();

    QString message = QString("<Tunnel> Cлушаю адрес %1:%2")
            .arg(mm_ipAddrServer.toString())
            .arg(mm_portServer);
    qDebug() << message;
}

bool Server::parsingAddressRemote(QString address, QHostAddress& addressRemote, quint16& port)
{
    qDebug() << "<Tunnel> Начат процесс определения remote_addr...";
    QStringList listGeneralAddr = address.split(":");
    QStringList listAddr = listGeneralAddr[0].split(".");

    if (!checkAddres(listGeneralAddr, listAddr) || !checkPort(listGeneralAddr))
        return false;

    addressRemote = QHostAddress(listGeneralAddr[0]);
    port = listGeneralAddr[1].toUInt();

    QString message = QString("<Tunnel> Пробрасываю пакеты с адреса %1:%2")
            .arg(addressRemote.toString())
            .arg(port);
    qDebug() << message;
    return true;
}

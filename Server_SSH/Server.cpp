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
    mm_socketServer(),
    mm_ipAddrServer(),
    mm_portServer(),
    mm_ipAddrRemote(),
    mm_portRemote(),
    mm_timer()
{
    qDebug() << "Вы запустили сервер";

    ptr_socketRemote = new QTcpSocket();

    // парсинг введеных значений
    parsingAddressServer(address);

    // запуск сервера
    mm_socketServer.listen(mm_ipAddrServer, mm_portServer);

    // соединение сигнал-слот
    connect(ptr_socketRemote, &QTcpSocket::readyRead,
            this, &Server::binaryMessageReceivedRemote);

    connect(&mm_socketServer, &QTcpServer::newConnection,
            this, &Server::newConnection, Qt::QueuedConnection);

    // таймер проверки клиентов
    mm_timer.setInterval(TIMERCOUNT);
    connect(&mm_timer, &QTimer::timeout,
            this, &Server::keppALive, Qt::QueuedConnection);
    mm_timer.start();
}

Server::~Server()
{
    mm_socketServer.close();
    ptr_socketRemote->close();
}

void Server::newConnection()
{
    QTcpServer *server = static_cast<QTcpServer*>(sender());

    QTcpSocket *client = server->nextPendingConnection();

    connect(client, &QTcpSocket::readyRead,
            this, &Server::binaryMessageReceived);

    connect(client, &QTcpSocket::disconnected,
            this, &Server::disconnected);

    QString message = QString("соединение открыто %1:%2").arg(
                QHostAddress(client->peerAddress().toIPv4Address()).toString(),
                QString::number(client->peerPort()));
    qDebug() << message;

    // добавление указателя на сокет в список контроля keepAlive
    quint64 nowTime = QDateTime::currentSecsSinceEpoch();
    mm_connectedList.insert(client, nowTime);
}

void Server::disconnected()
{
    QTcpSocket *client = static_cast<QTcpSocket*>(sender());

    QString message = QString("соединение закрыто %1:%2").arg(
                QHostAddress(client->peerAddress().toIPv4Address()).toString(),
                QString::number(client->peerPort()));
    qDebug() << message;

    // удаление сокета из списка контроля keepAlive
    mm_connectedList.remove(client);
}

void Server::binaryMessageReceived()
{
    QTcpSocket *client = static_cast<QTcpSocket*>(sender());
    QByteArray data = client->readAll();

    qDebug() << "Server принял от Local: |" << data << "|";

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

    if (mm_ipAddrRemote.toString().isEmpty() || mm_portRemote == 0)
    {
        if (!parsingAddressRemote(data))
            return;

        // соединение с адресом проброса
        ptr_socketRemote->connectToHost(mm_ipAddrRemote, mm_portRemote);
        return;
    }

    ptr_socketRemote->write(data);
    ptr_socketRemote->flush();
}

void Server::binaryMessageReceivedRemote()
{
    QByteArray data = ptr_socketRemote->readAll();
    qDebug() << "Server принял от Remote: |" << data << "|";
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
        }
    }
}

void Server::parsingAddressServer(QString address)
{
    qDebug() << "Начат процесс определения адреса прослушивания...";
    QStringList listGeneralAddr = address.split(":");
    QStringList listAddr = listGeneralAddr[0].split(".");

    if (!checkAddres(listGeneralAddr, listAddr) || !checkPort(listGeneralAddr))
        exit(EXIT_FAILURE);

    mm_ipAddrServer = QHostAddress(listGeneralAddr[0]);
    mm_portServer = listGeneralAddr[1].toUInt();

    QString message = QString("Server слушает адрес %1:%2")
            .arg(mm_ipAddrServer.toString())
            .arg(mm_portServer);
    qDebug() << message;
}

bool Server::parsingAddressRemote(QString address)
{
    qDebug() << "Начат процесс определения remote_addr...";
    QStringList listGeneralAddr = address.split(":");
    QStringList listAddr = listGeneralAddr[0].split(".");

    if (!checkAddres(listGeneralAddr, listAddr) || !checkPort(listGeneralAddr))
        return false;

    mm_ipAddrRemote = QHostAddress(listGeneralAddr[0]);
    mm_portRemote = listGeneralAddr[1].toUInt();

    QString message = QString("Server пробрасывает пакеты с адреса %1:%2")
            .arg(mm_ipAddrRemote.toString())
            .arg(mm_portRemote);
    qDebug() << message;
    return true;
}

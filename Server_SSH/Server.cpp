#include "Server.h"

Server::Server(QString address) :
    mm_server("Server", QWebSocketServer::NonSecureMode, this),
    mm_ipAaddr(),
    mm_port()
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

    mm_ipAaddr = QHostAddress(listGeneralAddr[0]);
    mm_port = listGeneralAddr[1].toUInt();

    QString message = QString("Server слушает адрес %1:%2")
            .arg(mm_ipAaddr.toString())
            .arg(mm_port);
    qDebug() << message;

    mm_server.listen(mm_ipAaddr, mm_port);

    // сигналы от ssl-севрера
    connect(&mm_server, &QWebSocketServer::newConnection,
            this, &Server::newConnection, Qt::QueuedConnection);

    connect(&mm_server, &QWebSocketServer::acceptError,
            this, &Server::acceptError, Qt::QueuedConnection);

    connect(&mm_server, &QWebSocketServer::serverError,
            this, &Server::serverError, Qt::QueuedConnection);
}

void Server::newConnection()
{
    QWebSocketServer *server = static_cast<QWebSocketServer*>(sender());

    QWebSocket *client = server->nextPendingConnection();

    // добавление указателя на сокет в список контроля keepAlive
    mm_connectedList.append(client);

    connect(client, &QWebSocket::binaryMessageReceived,
            this, &Server::binaryMessageReceived);

    connect(client, &QWebSocket::aboutToClose,
            this, &Server::aboutToClose);

    connect(client, &QWebSocket::disconnected,
            this, &Server::disconnected);
}

void Server::acceptError(QAbstractSocket::SocketError socketError)
{
    QWebSocketServer *server = static_cast<QWebSocketServer*>(sender());

    QString message = QString("ошибка подключения: #%1 %2").arg(
                QString::number(socketError),
                server->errorString());
    qDebug() << message;
}

void Server::serverError(QWebSocketProtocol::CloseCode closeCode)
{
    QWebSocketServer *server = static_cast<QWebSocketServer*>(sender());

    QString message = QString("ошибка сервера: #%1 %2").arg(
                QString::number(closeCode),
                server->errorString());
    qDebug() << message;
}

void Server::binaryMessageReceived(const QByteArray &data)
{
}

void Server::aboutToClose()
{
    QWebSocket *client = static_cast<QWebSocket*>(sender());

    QString message = QString("инициировано закрытие соединения %1:%2").arg(
                QHostAddress(client->peerAddress().toIPv4Address()).toString(),
                QString::number(client->peerPort()));
    qDebug() << message;

    // удаление сокета из списка контроля keepAlive
    mm_connectedList.removeOne(client);
}

void Server::disconnected()
{
    QWebSocket *client = static_cast<QWebSocket*>(sender());

    QString message = QString("соединение закрыто %1:%2").arg(
                QHostAddress(client->peerAddress().toIPv4Address()).toString(),
                QString::number(client->peerPort()));
    qDebug() << message;

    // удаление сокета из списка контроля keepAlive
    mm_connectedList.removeOne(client);
}

#include "RemoteServer.h"

RemoteServer::RemoteServer(QHostAddress address, quint16 port):
    mm_server()
{
    mm_server.listen(address, port);
    // соединение сигнал-слот
    connect(&mm_server, &QTcpServer::newConnection,
            this, &RemoteServer::newConnectionToRemote, Qt::QueuedConnection);
}

QTcpServer *RemoteServer::getServer()
{
    return &mm_server;
}

void RemoteServer::sendDataFromServer(QByteArray data)
{
    auto tcpSocket = mm_queue.dequeue();
    tcpSocket->write(data);
    tcpSocket->flush();
}

void RemoteServer::binaryMessageReceivedRemote()
{
    QTcpSocket *client = static_cast<QTcpSocket*>(sender());
    QByteArray data = client->readAll();

    mm_queue.enqueue(client);

    emit sendDataFromRemote(data);
}

void RemoteServer::newConnectionToRemote()
{
    QTcpServer *server = static_cast<QTcpServer*>(sender());

    QTcpSocket *client = server->nextPendingConnection();

    connect(client, &QTcpSocket::readyRead,
            this, &RemoteServer::binaryMessageReceivedRemote);

    connect(client, &QTcpSocket::disconnected,
            this, &RemoteServer::disconnected);

    QString message = QString("<Remote> соединение открыто %1:%2").arg(
                QHostAddress(client->peerAddress().toIPv4Address()).toString(),
                QString::number(client->peerPort()));
    qDebug() << message;
}

void RemoteServer::disconnected()
{
    QTcpSocket *client = static_cast<QTcpSocket*>(sender());

    QString message = QString("<Remote> соединение закрыто %1:%2").arg(
                QHostAddress(client->peerAddress().toIPv4Address()).toString(),
                QString::number(client->peerPort()));
    qDebug() << message;

    for(auto i = 0; i < mm_queue.count(); i++)
    {
        if (mm_queue[i] == client)
            mm_queue.removeAt(i);
    }
}

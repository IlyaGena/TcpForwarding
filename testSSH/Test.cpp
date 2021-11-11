#include "Test.h"

Test::Test(QString port):
    mm_server(this)
{
    // соединение сигнал-слот
    connect(&mm_server, &QTcpServer::newConnection,
            this, &Test::newConnectionToServer, Qt::QueuedConnection);
    mm_server.listen(QHostAddress("127.0.0.1"), port.toInt());
}

void Test::newConnectionToServer()
{
    QTcpServer *server = static_cast<QTcpServer*>(sender());

    mm_client = server->nextPendingConnection();

    connect(mm_client, &QTcpSocket::readyRead,
            this, &Test::binaryMessageReceivedServer);

    qDebug() << "Connect";
}

void Test::binaryMessageReceivedServer()
{
    QTcpSocket *client = static_cast<QTcpSocket*>(sender());
    qDebug() << "Пришло: " << client->readAll();
}

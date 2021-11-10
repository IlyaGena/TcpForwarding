#ifndef REMOTESERVER_H
#define REMOTESERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QQueue>

class RemoteServer : public QObject
{
    Q_OBJECT
public:
    RemoteServer(QHostAddress address, quint16 port);

    QTcpServer* getServer();

    void sendDataFromServer(QByteArray data);

signals:
    void sendDataFromRemote(QByteArray data);

private slots:
    /*!
     * \brief binaryMessageReceivedRemote приняты данные на remoteAddr
     * \param data данные
     */
    void binaryMessageReceivedRemote();
    /*!
     * \brief newConnectionToRemote появилось новое подключение на remote
     */
    void newConnectionToRemote();
    /*!
     * \brief disconnected клиент отключился
     */
    void disconnected();

private:
    QTcpServer              mm_server;
    QList<QTcpSocket*>      mm_listSockets;
    QQueue<QTcpSocket*>     mm_queue;
};

#endif // REMOTESERVER_H

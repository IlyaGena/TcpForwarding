#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QHostAddress>
#include <QTcpSocket>
#include <QSslSocket>
#include <QMap>
#include <QQueue>
#include <QDateTime>
#include <QTimer>

#include "RemoteServer.h"

#define TIMERCOUNT      1000
#define KEEPALIVE       350

class Server : public QObject
{
    Q_OBJECT
public:
    Server(QString address);

    ~Server();

private slots:

    /*!
     * \brief disconnected клиент отключился
     */
    void disconnected();

    /*!
     * \brief newConnectionToServer появилось новое подключение на server (от local)
     */
    void newConnectionToServer();

    /*!
     * \brief binaryMessageReceivedServer приняты данные от local_addr
     * \param data данные
     */
    void binaryMessageReceivedServer();

    void sendDataFromRemote(QByteArray data);

    /*!
     * \brief keppALive проверка списка подкл. клиентов
     */
    void keppALive();

private:
    /*!
     * \brief parsingAddressListen - парсинг адреса server введенного в cmd
     * \param address - значение из cmd
     */
    void parsingAddressServer(QString address);

    /*!
     * \brief parsingAddressRemote - парсинг адреса проброса присланного local_addr
     * \param address - значение от local_addr
     */
    bool parsingAddressRemote(QString address, QHostAddress &addressRemote, quint16 &port);

private:
    QTcpServer                      mm_server;              //!< объект сервера для прослушивания

    QHostAddress                    mm_ipAddrServer;        //!< адрес server
    quint16                         mm_portServer;          //!< порт server

    QMap<QTcpSocket*, quint64>      mm_connectedList;       //!< внутренний лист подключений для поддержания сессий
    QTimer                          mm_timer;               //!< таймер для keepALive

    QMap<QTcpSocket*, RemoteServer*> mm_forwardList;
};

#endif // SERVER_H

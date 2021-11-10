#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QHostAddress>
#include <QTcpSocket>
#include <QTcpServer>
#include <QMap>
#include <QDateTime>
#include <QTimer>

#define TIMERCOUNT      1000
#define KEEPALIVE       30

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
     * \brief newConnection появилось новое подключение
     */
    void newConnection();

    /*!
     * \brief binaryMessageReceived приняты данные от local_addr
     * \param data данные
     */
    void binaryMessageReceived();

    /*!
     * \brief binaryMessageReceived приняты данные от remoteAddr
     * \param data данные
     */
    void binaryMessageReceivedRemote();

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
    bool parsingAddressRemote(QString address);

private:
    QTcpServer                      mm_socketServer;        //!< объект сервера для прослушивания
    QTcpSocket*                     ptr_socketRemote;       //!< указатель сокета с remoteAddr

    QHostAddress                    mm_ipAddrServer;        //!< адрес server
    quint16                         mm_portServer;          //!< порт server

    QHostAddress                    mm_ipAddrRemote;         //!< адрес remoteAddr
    quint16                         mm_portRemote;          //!< порт remoteAddr

    QMap<QTcpSocket*, quint64>      mm_connectedList;       //!< внутренний лист подключений для поддержания сессий
    QTimer                          mm_timer;               //!< таймер для keepALive
};

#endif // SERVER_H

#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QSslSocket>
#include <QHostAddress>
#include <QDebug>
#include <QTimer>
#include <QUrl>

#define RECONNECT           1000

class Client : public QObject
{
    Q_OBJECT
public:
    Client(QString cmd, QString addrServer);

    ~Client();
private:
    /*!
     * \brief parsingAddressListen - парсинг адреса сервера
     * \param address - значение из cmd
     */
    void parsingAddressServer(QString address);
    /*!
     * \brief parsingAddresses - парсинг адреса local_addr и remoteAddr
     * \param cmd - значение из cmd
     */
    void parsingAddresses(QString cmd);
    /*!
     * \brief send - отправка данных на сервер
     * \param data - данные
     */
    void send(QByteArray data);

private slots:
    /*!
     * \brief binaryMessageReceived - принятые данные на local_addr
     * \param data - данные
     */
    void binaryMessageReceivedServer();
    /*!
     * \brief binaryMessageReceived - принятые данные на local_addr
     * \param data - данные
     */
    void binaryMessageReceivedLocal();
    /*!
     * \brief stateChanged - изменение состояния сокета server
     * \param state - состояние
     */
    void stateChanged(QAbstractSocket::SocketState state);
    /*!
     * \brief stateChanged - изменение состояния сокета server
     * \param state - состояние
     */
    void stateChangedClient(QAbstractSocket::SocketState state);
    /*!
     * \brief reConnection - изменить состояние сокета server
     */
    void reConnection();
    /*!
     * \brief reConnection - изменить состояние сокета server
     */
    void reConnectionClient();

private:
    QSslSocket*                     ptr_socketServer;       //!< сокет до сервера
    QTcpSocket*                     ptr_socketLocal;        //!< сокет до ресура

    QHostAddress                    mm_ipAddrServer;        //!< адрес сервера
    quint16                         mm_portServer;          //!< порт сервера

    QHostAddress                    mm_ipAddrLocal;         //!< адрес ресура
    quint16                         mm_portLocal;           //!< порт ресура

    QHostAddress                    mm_ipAddrRemote;        //!< адрес клиента
    quint16                         mm_portRemote;          //!< порт клиента

    QTimer                          mm_timer;               //!< таймер переподключения
    QTimer                          mm_timerClient;         //!< таймер переподключения к ресурсу
    QList<QByteArray>               mm_queue;               //!< очередь сообщений

    QByteArray                      mm_remoteAddress;       //!< данные для сервера
};

#endif // CLIENT_H

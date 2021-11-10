#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
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
     * \brief reConnection - изменить состояние сокета server
     */
    void reConnection();

private:
    QTcpSocket                      tmp_socketResource;         //!< сокет до ресура
    QTcpSocket                      mm_socketServer;        //!< сокет до сервера
    QTcpSocket                      mm_socketLocal;         //!< сокет до ресура

    QHostAddress                    mm_ipAddrServer;        //!< адрес сервера
    quint16                         mm_portServer;          //!< порт сервера
    QUrl                            mm_urlServer;           //!< url сервера

    QHostAddress                    mm_ipAddrLocal;         //!< адрес ресура
    quint16                         mm_portLocal;           //!< порт ресура
    QUrl                            mm_urlLocal;            //!< url ресура

    QHostAddress                    mm_ipAddrRemote;        //!< адрес клиента
    quint16                         mm_portRemote;          //!< порт клиента

    QTimer                          mm_timer;               //!< таймер переподключения
    QList<QByteArray>               mm_queue;               //!< очередь сообщений
    QTimer                          tmp_timer;               //!< таймер переподключения
};

#endif // CLIENT_H

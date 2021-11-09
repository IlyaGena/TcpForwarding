#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QHostAddress>
#include <QWebSocketServer>
#include <QWebSocket>

class Server : public QObject
{
    Q_OBJECT
public:
    Server(QString address);

private slots:
    /*!
     * \brief newConnection появилось новое подключение
     */
    void newConnection();

    /*!
     * \brief acceptError сетевая ошибка при создании нового подключения
     * \param socketError тип ошибки
     */
    void acceptError(QAbstractSocket::SocketError socketError);

    /*!
     * \brief serverError ошибка сервера
     * \param closeCode тип ошибки
     */
    void serverError(QWebSocketProtocol::CloseCode closeCode);

    /*!
     * \brief binaryMessageReceived приняты данные от клиента
     * \param data данные
     */
    void binaryMessageReceived(const QByteArray &data);

    /*!
     * \brief aboutToClose клиент отключается
     */
    void aboutToClose();

    /*!
     * \brief disconnected клиент отключился
     */
    void disconnected();

private:
    QWebSocketServer mm_server;
    QHostAddress mm_ipAaddr;
    quint16 mm_port;
    QList<QWebSocket*> mm_connectedList;    //!< внутренний лист подключений для поддержания сессий
};

#endif // SERVER_H

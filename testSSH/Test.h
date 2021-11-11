#ifndef TEST_H
#define TEST_H

#include <QObject>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>

class Test : public QObject
{
    Q_OBJECT
public:
    Test(QString port);

private slots:
    /*!
     * \brief newConnectionToServer появилось новое подключение на server (от local)
     */
    void newConnectionToServer();
    /*!
     * \brief binaryMessageReceivedServer приняты данные от local_addr
     * \param data данные
     */
    void binaryMessageReceivedServer();

private:
    QTcpServer      mm_server;
    QTcpSocket*     mm_client;
};

#endif // TEST_H

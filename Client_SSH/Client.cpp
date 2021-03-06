#include "Client.h"

inline bool checkAddres(QStringList listGeneralAddr, QStringList listAddr)
{
    foreach (auto record, listAddr)
    {
        auto recordInt = record.toInt();
        if  (recordInt < 0 || recordInt > 255)
        {
            qDebug() << "Адрес вышел за пределы допустимых значений";
            return false;
        }
    }

    if (listGeneralAddr.count() != 2 ||
            listAddr.count() != 4)
    {
        qDebug() << "Адрес не соответствует шаблону adress:port";
        return false;
    }
    return true;
}

inline bool checkPort(QStringList listGeneralAddr)
{
    if (listGeneralAddr[1].toInt() < 0 || listGeneralAddr[1].toInt() > 65535)
    {
        qDebug() << "Порт вышел за пределы допустимых значений";
        return false;
    }
    return true;
}

Client::Client(QString cmd, QString addrServer) :
    mm_timer(this),
    mm_timerClient(this)
{
    ptr_socketServer = new QSslSocket();
    ptr_socketLocal = new QTcpSocket();
    qDebug() << "Вы запустили клиент";

    // парсинг введеных значений
    parsingAddressServer(addrServer);
    parsingAddresses(cmd);

    // соединение сигнал-слот
    connect(ptr_socketLocal, &QTcpSocket::readyRead,
            this, &Client::binaryMessageReceivedLocal);

    connect(ptr_socketLocal, &QTcpSocket::stateChanged,
            this, &Client::stateChangedClient);

    connect(ptr_socketServer, &QTcpSocket::readyRead,
            this, &Client::binaryMessageReceivedServer);

    connect(ptr_socketServer, &QTcpSocket::stateChanged,
            this, &Client::stateChanged);

    // открытие соединение с сервером
    ptr_socketServer->connectToHost(mm_ipAddrServer, mm_portServer);

    // отправляем данные о клиенте
    mm_remoteAddress = mm_ipAddrRemote.toString().toUtf8() + ":" + QString::number(mm_portRemote).toUtf8();

    // таймер переподключения
    mm_timer.setInterval(RECONNECT);
    mm_timerClient.setInterval(RECONNECT);
    connect(&mm_timer, &QTimer::timeout, this, &Client::reConnection);
    connect(&mm_timerClient, &QTimer::timeout, this, &Client::reConnectionClient);
}

Client::~Client()
{
    ptr_socketServer->close();
    ptr_socketLocal->close();
}

void Client::parsingAddressServer(QString address)
{
    qDebug() << "Начат процесс определения адреса server...";
    QStringList listGeneralAddr = address.split(":");
    QStringList listAddr = listGeneralAddr[0].split(".");

    if (!checkAddres(listGeneralAddr, listAddr) || !checkPort(listGeneralAddr))
        exit(EXIT_FAILURE);

    mm_ipAddrServer = QHostAddress(listGeneralAddr[0]);
    mm_portServer = listGeneralAddr[1].toUInt();

    QString message = QString("Server определен по адресу %1:%2")
            .arg(mm_ipAddrServer.toString())
            .arg(mm_portServer);
    qDebug() << message;
}

void Client::parsingAddresses(QString cmd)
{
    qDebug() << "Начат процесс определения адреса ресурса и клиента...";
    QStringList listGeneralAddr = cmd.split(":");

    if (listGeneralAddr.count() != 4)
    {
        qDebug() << "Несовпадение по формату: '<remote_ip>:<remote_port:><local_ip>:<local_port>'";
        exit(EXIT_FAILURE);
    }

    QStringList listGeneralClient;
    QStringList listAddrClient =listGeneralAddr[0].split(".") ;
    listGeneralClient.append(listGeneralAddr[0]);
    listGeneralClient.append(listGeneralAddr[1]);

    if (!checkAddres(listGeneralClient, listAddrClient) || !checkPort(listGeneralClient))
        exit(EXIT_FAILURE);

    mm_ipAddrRemote = QHostAddress(listGeneralAddr[0]);
    mm_portRemote = listGeneralAddr[1].toInt();

    QString message = QString("Remote определен по адресу %1:%2")
            .arg(mm_ipAddrRemote.toString())
            .arg(mm_portRemote);
    qDebug() << message;


    QStringList listGeneralLocal;
    QStringList listAddrLocal =listGeneralAddr[2].split(".") ;
    listGeneralLocal.append(listGeneralAddr[2]);
    listGeneralLocal.append(listGeneralAddr[3]);

    if (!checkAddres(listGeneralLocal, listAddrLocal) || !checkPort(listGeneralLocal))
        exit(EXIT_FAILURE);

    mm_ipAddrLocal = QHostAddress(listGeneralAddr[2]);
    mm_portLocal = listGeneralAddr[3].toInt();

    ptr_socketLocal->connectToHost(mm_ipAddrLocal, mm_portLocal);

    message = QString("Local определен по адресу %1:%2")
            .arg(mm_ipAddrLocal.toString())
            .arg(mm_portLocal);
    qDebug() << message;
}

void Client::send(QByteArray data)
{
    if (ptr_socketServer->state() != QAbstractSocket::ConnectedState)
    {
        mm_queue.append(data);
        return;
    }

    ptr_socketServer->write(data);
    ptr_socketServer->flush();
}

void Client::binaryMessageReceivedServer()
{
    QByteArray data = ptr_socketServer->readAll();
    qDebug() << "Local принял от Server: <" << data << ">";

    ptr_socketLocal->write(data);
    ptr_socketLocal->flush();
}

void Client::binaryMessageReceivedLocal()
{
    QTcpSocket *client = static_cast<QTcpSocket*>(sender());
    QByteArray data = client->readAll();

    qDebug() << "Local принял от Client: <>";
    send(data);
}

void Client::stateChanged(QAbstractSocket::SocketState state)
{
    if (state == QAbstractSocket::ConnectedState && !mm_queue.isEmpty())
    {
        while (!mm_queue.isEmpty())
        {
            auto data = mm_queue.first();
            ptr_socketServer->write(data);
            ptr_socketServer->flush();

            mm_queue.removeFirst();
        }
    }
    // если подключения нет
    if (state == QAbstractSocket::UnconnectedState && !mm_timer.isActive())
        mm_timer.start();

    // если подключение появилось
    if (state == QAbstractSocket::ConnectedState)
    {
        send(mm_remoteAddress);
        mm_timer.stop();
    }
}

void Client::stateChangedClient(QAbstractSocket::SocketState state)
{
    // если подключения нет
    if (state == QAbstractSocket::UnconnectedState && !mm_timer.isActive())
        mm_timerClient.start();

    // если подключение появилось
    if (state == QAbstractSocket::ConnectedState)
        mm_timerClient.stop();
}

void Client::reConnection()
{
    qDebug() << "Connecting to server...";
    // проверка на то, какое сейчас подключение
    if (ptr_socketServer->state() != QAbstractSocket::ConnectedState)
        ptr_socketServer->connectToHost(mm_ipAddrServer, mm_portServer);
}

void Client::reConnectionClient()
{
    qDebug() << "Connecting to Client...";
    // проверка на то, какое сейчас подключение
    if (ptr_socketLocal->state() != QAbstractSocket::ConnectedState)
        ptr_socketLocal->connectToHost(mm_ipAddrLocal, mm_portLocal);
}

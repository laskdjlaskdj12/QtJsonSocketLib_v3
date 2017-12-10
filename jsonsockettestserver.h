#ifndef JSONSOCKETTESTSERVER_H
#define JSONSOCKETTESTSERVER_H

#include <QObject>
#include <QTcpServer>
#include "Dependency/qt_json_socket_lib.h"
#include <QDebug>

class TestServer: public QObject
{
    Q_OBJECT
public:

    TestServer()
    {
        server = QSharedPointer<QTcpServer>::create();

        connect(server.data(), SIGNAL(newConnection()), this, SLOT(onConnect()));
    }

    ~TestServer()
    {
        server.clear();

        disconnect(server.data(), SIGNAL(newConnection()), this, SLOT(onConnect()));
    }


    QSharedPointer<QTcpSocket> GetSocket()
    {
        return socket;
    }

signals:
    void OnConnectSignal();
    void OnRecvSignal(QByteArray);
    void OnSendSingal();
    void OnDisconnectSignal();

public slots:
    inline void onStart()
    {
        server->listen(QHostAddress::Any, 7071);
    }

    inline void onFinished()
    {
        if(socket.isNull() == false)
        {
            socket->disconnectFromHost();
            socket.clear();
        }

        server->close();
    }

    inline void onConnect()
    {
        socket = QSharedPointer<QTcpSocket>(server->nextPendingConnection());

        if(socket.isNull() )
        {
            qDebug()<<"QTCpSocket Alloc Error";
            return;
        }

        connect(socket.data(), SIGNAL(readyRead()), this, SLOT(onRecv()));
        connect(socket.data(), SIGNAL(disconnected()), this, SLOT(onDisconnect()));

        emit OnConnectSignal();
        qDebug()<<"Connect is clear";
    }

    inline void onRecv()
    {
        QByteArray ReadBase64Size = socket->read(sizeof(unsigned int));

        qDebug()<<"Read Size : " + QString::number (ReadBase64Size.toInt());

        unsigned int ReadSize = static_cast<unsigned int>(ReadBase64Size.toInt());

        QByteArray Base64Source = socket->read(ReadSize);

        emit OnRecvSignal(Base64Source);

        if(Base64Source.isNull())
        {
            qDebug()<<"Can't Read Base64Source : " + socket->errorString();
            return;
        }

        QByteArray ReadJsonSource = QByteArray::fromBase64(Base64Source);

        if(ReadJsonSource.isEmpty())
        {
            qDebug()<<"[Debug] : Recv QByteArray is fail : " + socket->errorString();
            return;
        }

        QJsonDocument RecvJsonDoc = QJsonDocument::fromJson(ReadJsonSource);

        if(RecvJsonDoc.isEmpty())
        {
            qDebug()<<"[Debug] : Recv JsonDoc is fail : " + socket->errorString();
            return;
        }

        RecvRequest = RecvJsonDoc.object();

        if(RecvRequest.find("Reply") == RecvRequest.end())
        {
            qDebug()<<"[Debug] :RecvByteArray : " + RecvJsonDoc.toJson();
            return;
        }

        if(RecvRequest["Reply"].toBool() == true)
        {
            qDebug()<<"[Debug] :Send Reply : " + RecvJsonDoc.toJson();

            emit OnSendSingal();

            if (socket->write(QString::number(Base64Source.size()).toUtf8(), sizeof(unsigned int)) < 0
                    || socket->write(Base64Source) < 0)
            {
                qDebug()<<"[Debug] : Socket Error" + socket->errorString();
            }
        }
    }

    inline void onDisconnect()
    {
       emit OnDisconnectSignal();
    }

private:
    QJsonObject RecvRequest;

    QSharedPointer<QTcpServer> server;
    QSharedPointer<QTcpSocket> socket;
};

#endif // JSONSOCKETTESTSERVER_H

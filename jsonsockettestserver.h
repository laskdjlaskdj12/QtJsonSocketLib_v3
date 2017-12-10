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
    }

    ~TestServer()
    {
        server->close();
    }

    inline QSharedPointer<Qt_Json_Socket_Lib> GetConnectSocket()
    {
        return lib;
    }

    inline QJsonObject GetRecvObj()
    {
        return RecvRequest;
    }

public slots:
    inline void onStart()
    {
        server = QSharedPointer<QTcpServer>::create();

        connect(server.data(), SIGNAL(newConnection()), this, SLOT(onConnect()));

        server->listen(QHostAddress::Any, 7071);
    }

    inline void onConnect()
    {
        if(!lib.isNull())
        {
            if(lib->is_connect())
            {
                lib->disconnect_socket();
            }

            lib->delete_QTcpSocket();

            lib.clear();
        }

        lib = QSharedPointer<Qt_Json_Socket_Lib>::create();
        lib->set_socket(server->nextPendingConnection());

        lib->set_connect_timeout(2000);
        lib->set_recv_timeout(-1);
        lib->set_send_timeout(2000);

        connect(lib->get_socket(), SIGNAL(disconnected()), this, SLOT(onDisconnect()));

        onRecv();

        qDebug()<<"Connect is clear";
    }

    inline void onRecv()
    {
        qDebug()<<"Recv by Request Socket";

        lib->get_socket()->waitForBytesWritten(-1);

        QByteArray SizeByte = lib->get_socket()->read(sizeof(unsigned int));

        unsigned int readSize = static_cast<unsigned int>(SizeByte.toInt());

        QByteArray ReadBase64Source = lib->get_socket()->read(readSize);

        QByteArray ReadJsonSource = QByteArray::fromBase64(ReadBase64Source);

        if(ReadJsonSource.isEmpty())
        {
            qDebug()<<"[Debug] : Recv QByteArray is fail : " + lib->get_socket()->errorString();
            return;
        }

        QJsonDocument RecvJsonDoc = QJsonDocument::fromJson(ReadJsonSource);

        if(RecvJsonDoc.isEmpty())
        {
            qDebug()<<"[Debug] : Recv JsonDoc is fail : " + lib->get_socket()->errorString();
            return;
        }

        RecvRequest = RecvJsonDoc.object();

        if(RecvRequest.find("Reply") == RecvRequest.end())
        {
            qDebug()<<"[Debug] :RecvByteArray : " + RecvJsonDoc.toJson();
        }

        if(RecvRequest["Reply"].toBool() == true)
        {
            lib->send_Json(RecvRequest);
        }
    }

    inline void onDisconnect()
    {
        if(lib.isNull())
        {
            return;
        }

        if(!lib->get_socket())
        {
            return;
        }

        if(lib->is_connect())
        {
            lib->disconnect_socket();
        }
    }

private:
    QJsonObject RecvRequest;

    QSharedPointer<QTcpServer> server;
    QSharedPointer<Qt_Json_Socket_Lib> lib;
};

#endif // JSONSOCKETTESTSERVER_H

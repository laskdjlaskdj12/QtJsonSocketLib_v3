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

        bool NewConnectionSignalBind = connect(server.data(), SIGNAL(newConnection()), this, SLOT(onConnect()));

        if(NewConnectionSignalBind == false)
        {
            qDebug()<<"NewConnectionSignal false";
            return;
        }

        server->listen(QHostAddress::Any, 7071);

        if(server->isListening() == false)
        {
            qDebug()<<"Server is not listening";
            return;
        }
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
        lib->set_recv_timeout(2000);
        lib->set_send_timeout(2000);

        connect(lib->get_socket(), SIGNAL(disconnected()), this, SLOT(onDisconnect()));
        connect(lib->get_socket(), SIGNAL(readyRead()), this, SLOT(onRecv()));

        qDebug()<<"Connect is clear";

    }

    inline void onRecv()
    {
        QJsonDocument RecvJsonDoc = lib->recv_Json();

        if(RecvJsonDoc.isEmpty())
        {
            qDebug()<<"[Debug] : Recv JsonDoc is fail";
        }

        RecvRequest = RecvJsonDoc.object();

        if(RecvRequest.find("Reply") != RecvRequest.end())
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
        lib->disconnect_socket();
        lib->delete_QTcpSocket();
        lib.clear();
    }

private:
    QJsonObject RecvRequest;

    QSharedPointer<QTcpServer> server;
    QSharedPointer<Qt_Json_Socket_Lib> lib;
};

#endif // JSONSOCKETTESTSERVER_H

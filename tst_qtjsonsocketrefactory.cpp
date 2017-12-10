#include <QtTest>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QThread>
#include <QCoreApplication>
#include "Dependency/qt_json_socket_lib.h"
#include "qtjsonsocketlib_v3.h"
#include "jsonsockettestserver.h"

// add necessary includes here

class QtJsonSocketRefactory : public QObject
{
    Q_OBJECT

public:
    QtJsonSocketRefactory()
    {
        TestTemplateServer = QSharedPointer<TestServer>::create();
        TestThread = QSharedPointer<QThread>::create();

        TestTemplateServer->moveToThread(TestThread.data());
        connect(TestThread.data(), SIGNAL(started()), TestTemplateServer.data(), SLOT(onStart()));
        connect(TestThread.data(), SIGNAL(finished()), TestTemplateServer.data(), SLOT(onFinished()));
        TestThread->start();

        QtJsonSocketLib::DebugLog::ShowDebugMessage = true;
    }
    ~QtJsonSocketRefactory()
    {
        TestThread->exit();
        TestTemplateServer.clear();
        TestThread.clear();

        TestTemplateServer.clear();
    }

protected:
    QSharedPointer<TestServer> TestTemplateServer;
    QSharedPointer<QThread> TestThread;


private slots:

    inline void AllocSocketTest()
    {
        QSharedPointer<QtJsonSocketLib_v3> Socket = QSharedPointer<QtJsonSocketLib_v3>::create(true, false);

        QVERIFY(Socket->make_QTcpSocket());

        QVERIFY(Socket->get_socket() != nullptr);

        Socket.clear();
    }

    inline void ConnectTest()
    {
        QSignalSpy ConnectSignal(TestTemplateServer.data(), &TestServer::OnConnectSignal);
        QSignalSpy DisconnectSignal(TestTemplateServer.data(), &TestServer::OnDisconnectSignal);

        QSharedPointer<QtJsonSocketLib_v3> Socket = QSharedPointer<QtJsonSocketLib_v3>::create(true, false);

        QVERIFY(Socket->make_QTcpSocket());

        QVERIFY(Socket->get_socket() != nullptr);

        Socket->connect_server("127.0.0.1", 7071);

        QVERIFY(Socket->is_connect());

        QVERIFY(ConnectSignal.wait(1000));

        Socket->get_socket()->disconnectFromHost();

        QVERIFY(DisconnectSignal.wait(1000) );

        Socket.clear();
    }

    inline void DisconnectTest()
    {
        QSignalSpy ConnectSignal(TestTemplateServer.data(), &TestServer::OnConnectSignal);
        QSignalSpy DisconnectSignal(TestTemplateServer.data(), &TestServer::OnDisconnectSignal);

        QSharedPointer<QtJsonSocketLib_v3> Socket = QSharedPointer<QtJsonSocketLib_v3>::create(true, false);

        QVERIFY(Socket->make_QTcpSocket());

        QVERIFY(Socket->get_socket() != nullptr);

        Socket->connect_server("127.0.0.1", 7071);

        QVERIFY(Socket->is_connect());

        QVERIFY(ConnectSignal.wait(1000));

        Socket->disconnect_socket();

        QVERIFY(DisconnectSignal.wait(1000));

        Socket.clear();
    }

    inline void SendTest()
    {
        QSignalSpy ConnectSignal(TestTemplateServer.data(), &TestServer::OnConnectSignal);
        QSignalSpy DisconnectSignal(TestTemplateServer.data(), &TestServer::OnDisconnectSignal);
        QSignalSpy RecvSignal(TestTemplateServer.data(), &TestServer::OnRecvSignal);
        QSignalSpy SendSignal(TestTemplateServer.data(), &TestServer::OnSendSingal);

        QSharedPointer<QtJsonSocketLib_v3> Socket = QSharedPointer<QtJsonSocketLib_v3>::create(true, false);

        QVERIFY(Socket->make_QTcpSocket());

        QVERIFY(Socket->get_socket() != nullptr);

        Socket->connect_server("127.0.0.1", 7071);

        QVERIFY(Socket->is_connect());

        QVERIFY(ConnectSignal.wait(1000));


        QJsonObject SendObject;

        SendObject["TestSend"] = true;

        QVERIFY(Socket->send_Json(SendObject));

        QVERIFY(RecvSignal.wait(1000));



        QList<QVariant> RecvSignalFromTestServer = RecvSignal.takeFirst();

        QVERIFY (!RecvSignalFromTestServer.at(0).toByteArray().isEmpty());

        QJsonDocument Doc = QJsonDocument::fromJson(QByteArray::fromBase64(RecvSignalFromTestServer.at(0).toByteArray()));

        QVERIFY (!Doc.isEmpty());

        QVERIFY (Doc.object() == SendObject);

        Socket.clear();
    }

    inline void RecvTest()
    {
        QSignalSpy ConnectSignal(TestTemplateServer.data(), &TestServer::OnConnectSignal);
        QSignalSpy DisconnectSignal(TestTemplateServer.data(), &TestServer::OnDisconnectSignal);
        QSignalSpy RecvSignal(TestTemplateServer.data(), &TestServer::OnRecvSignal);
        QSignalSpy SendSignal(TestTemplateServer.data(), &TestServer::OnSendSingal);

        QSharedPointer<QtJsonSocketLib_v3> Socket = QSharedPointer<QtJsonSocketLib_v3>::create(true, false);

        QVERIFY(Socket->make_QTcpSocket());

        QVERIFY(Socket->get_socket() != nullptr);

        Socket->connect_server("127.0.0.1", 7071);

        QVERIFY(Socket->is_connect());

        QVERIFY(ConnectSignal.wait(1000));

        QVERIFY(TestTemplateServer->GetSocket()->state() == QAbstractSocket::ConnectedState);


        QJsonObject SendObject;

        SendObject["Reply"] = true;

        QVERIFY(Socket->send_Json(SendObject));

        QVERIFY(RecvSignal.wait());

        if (SendSignal.wait() == false)
        {
            QVERIFY(SendSignal.count() > 0);
        }

        QJsonObject RecvJson = Socket->RecvJson();

        QVERIFY(RecvJson.isEmpty() == false);

        QVERIFY(RecvJson == SendObject);

        Socket->disconnect_socket();

        Socket.clear();
    }

    inline void RecvSignalTest()
    {
        QSharedPointer<QtJsonSocketLib_v3> Socket = QSharedPointer<QtJsonSocketLib_v3>::create(false, true);

        QSignalSpy ConnectSignal(TestTemplateServer.data(), &TestServer::OnConnectSignal);
        QSignalSpy DisconnectSignal(TestTemplateServer.data(), &TestServer::OnDisconnectSignal);
        QSignalSpy RecvSignal(TestTemplateServer.data(), &TestServer::OnRecvSignal);
        QSignalSpy SendSignal(TestTemplateServer.data(), &TestServer::OnSendSingal);

        QSignalSpy RecvSignalSpy (Socket.data(), &QtJsonSocketLib_v3::OnRecvEvent);

        QVERIFY(Socket->make_QTcpSocket());

        QVERIFY(Socket->get_socket() != nullptr);

        Socket->connect_server("127.0.0.1", 7071);

        QVERIFY(Socket->is_connect());

        QVERIFY(ConnectSignal.wait(1000));


        QJsonObject SendObject;

        SendObject["Reply"] = true;

        QVERIFY(Socket->send_Json(SendObject));

        QVERIFY(RecvSignal.wait(1000));

        if (SendSignal.wait() == false)
        {
            QVERIFY(SendSignal.count() > 0);
        }

        if (RecvSignalSpy.wait() == false)
        {
            QVERIFY(RecvSignalSpy.count() > 0);
        }

        Socket->disconnect_server();

        Socket.clear();
    }

    inline void DisconnectSignalTest()
    {
        QSharedPointer<QtJsonSocketLib_v3> Socket = QSharedPointer<QtJsonSocketLib_v3>::create(false, true);

        QSignalSpy ConnectSignal(TestTemplateServer.data(), &TestServer::OnConnectSignal);
        QSignalSpy DisconnectSignal(TestTemplateServer.data(), &TestServer::OnDisconnectSignal);
        QSignalSpy RecvSignal(TestTemplateServer.data(), &TestServer::OnRecvSignal);
        QSignalSpy SendSignal(TestTemplateServer.data(), &TestServer::OnSendSingal);

        QSignalSpy RecvSignalSpy (Socket.data(), &QtJsonSocketLib_v3::OnRecvEvent);
        QSignalSpy DisconnectSignalSpy (Socket.data(), &QtJsonSocketLib_v3::OnDisconnectEvent);


        QVERIFY(Socket->make_QTcpSocket());

        QVERIFY(Socket->get_socket() != nullptr);

        Socket->connect_server("127.0.0.1", 7071);

        QVERIFY(Socket->is_connect());

        QVERIFY(ConnectSignal.wait(1000));


        QJsonObject SendObject;

        SendObject["Reply"] = true;

        QVERIFY(Socket->send_Json(SendObject));

        QVERIFY(RecvSignal.wait(1000));

        if (SendSignal.wait() == false)
        {
            QVERIFY(SendSignal.count() > 0);
        }

        if (RecvSignalSpy.wait(1000) == false)
        {
            QVERIFY(RecvSignalSpy.count() > 0);
        }

        Socket->disconnect_server();

        if (DisconnectSignalSpy.wait(1000) == false)
        {
            QVERIFY(DisconnectSignalSpy.count() > 0);
        }

        Socket.clear();
    }
};

QTEST_MAIN(QtJsonSocketRefactory)

#include "tst_qtjsonsocketrefactory.moc"

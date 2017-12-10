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

        TestThread->start();

        QtJsonSocketLib::DebugLog::ShowDebugMessage = true;
    }
    ~QtJsonSocketRefactory()
    {
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
        QSharedPointer<QtJsonSocketLib_v3> Socket = QSharedPointer<QtJsonSocketLib_v3>::create(true, false);

        QVERIFY(Socket->make_QTcpSocket());

        QVERIFY(Socket->get_socket() != nullptr);

        Socket->connect_server("127.0.0.1", 7071);

        QVERIFY(Socket->is_connect());

        QThread::msleep(100);

        QVERIFY(!TestTemplateServer->GetConnectSocket().isNull());

        QVERIFY(TestTemplateServer->GetConnectSocket()->is_connect());

        Socket->disconnect_server();
        Socket.clear();
    }

    inline void DisconnectTest()
    {
        QSharedPointer<QtJsonSocketLib_v3> Socket = QSharedPointer<QtJsonSocketLib_v3>::create(true, false);

        QVERIFY(Socket->make_QTcpSocket());

        QVERIFY(Socket->get_socket() != nullptr);

        Socket->connect_server("127.0.0.1", 7071);

        QVERIFY(Socket->is_connect());

        QThread::msleep(100);

        QVERIFY(!TestTemplateServer->GetConnectSocket().isNull());

        QVERIFY(TestTemplateServer->GetConnectSocket()->is_connect());

        Socket->disconnect_socket();

        QVERIFY(TestTemplateServer->GetConnectSocket().isNull());

        Socket.clear();
    }

    inline void SendTest()
    {
        QSharedPointer<QtJsonSocketLib_v3> Socket = QSharedPointer<QtJsonSocketLib_v3>::create(true, false);

        QVERIFY(Socket->make_QTcpSocket());

        QVERIFY(Socket->get_socket() != nullptr);

        Socket->connect_server("127.0.0.1", 7071);

        QVERIFY(Socket->is_connect());

        QJsonObject SendObject;

        SendObject["TestSend"] = true;

        QVERIFY(Socket->send_Json(SendObject));

        QThread::msleep(100);

        QVERIFY(!TestTemplateServer->GetConnectSocket().isNull());

        QVERIFY(TestTemplateServer->GetRecvObj() == SendObject);

        Socket.clear();
    }

    inline void RecvTest()
    {
        QSharedPointer<QtJsonSocketLib_v3> Socket = QSharedPointer<QtJsonSocketLib_v3>::create(true, false);

        QVERIFY(Socket->make_QTcpSocket());

        QVERIFY(Socket->get_socket() != nullptr);

        Socket->connect_server("127.0.0.1", 7071);

        QVERIFY(Socket->is_connect());

        QJsonObject SendObject;

        SendObject["Reply"] = true;

        QVERIFY(Socket->send_Json(SendObject));

        QJsonObject RecvObject;

        RecvObject = Socket->RecvJson();

        QVERIFY(!RecvObject.isEmpty());

        QVERIFY(RecvObject == SendObject);

        QVERIFY(!TestTemplateServer->GetConnectSocket().isNull());

        QVERIFY(TestTemplateServer->GetRecvObj() == SendObject);

        Socket.clear();
    }

    inline void RecvSignalTest()
    {
        QSharedPointer<QtJsonSocketLib_v3> Socket = QSharedPointer<QtJsonSocketLib_v3>::create(false, true);

        QSignalSpy RecvSignalSpy (Socket.data(), SIGNAL(OnRecvEvent()));

        QVERIFY(Socket->make_QTcpSocket());

        QVERIFY(Socket->get_socket() != nullptr);

        Socket->connect_server("127.0.0.1", 7071);

        QVERIFY(Socket->is_connect());

        QJsonObject SendObject;

        SendObject["Reply"] = true;

        QVERIFY(Socket->send_Json(SendObject));

        QVERIFY(RecvSignalSpy.wait());

        Socket.clear();
    }

    inline void DisconnectSignalTest()
    {
        QSharedPointer<QtJsonSocketLib_v3> Socket = QSharedPointer<QtJsonSocketLib_v3>::create(false, true);

        QSignalSpy DisconnectSignalSpy (Socket.data(), SIGNAL(OnDisconnectEvent()));

        QVERIFY(Socket->make_QTcpSocket());

        QVERIFY(Socket->get_socket() != nullptr);

        Socket->connect_server("127.0.0.1", 7071);

        QVERIFY(Socket->is_connect());

        QJsonObject SendObject;

        SendObject["Disconnect"] = true;

        QVERIFY(Socket->send_Json(SendObject));

        QVERIFY(DisconnectSignalSpy.wait());

        Socket.clear();
    }
};

QTEST_MAIN(QtJsonSocketRefactory)

#include "tst_qtjsonsocketrefactory.moc"

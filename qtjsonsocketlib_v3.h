#ifndef QTJSONSOCKETLIB_V3_H
#define QTJSONSOCKETLIB_V3_H

#include <QObject>
#include <QTcpSocket>
#include <QMetaMethod>
#include <QSharedPointer>
#include <QDebug>
#include <QByteArray>

#include <QJsonObject>
#include <QJsonDocument>
#include "Dependency/qt_json_socket_lib.h"

namespace QtJsonSocketLib
{

class DebugLog :public QObject
{
    Q_OBJECT
public:
    static inline void ShowDebug(const QString& str, unsigned int line, QString FunctionName, QString FileName)
    {
        if(ShowDebugMessage == true)
        {
            qDebug()<<"[Debug] : " + FileName + " : " + FunctionName + ":" + QString::number(line) + " : " + str;
        }
    }

    static bool ShowDebugMessage;
};

}
#define DEBUGLOG(X) QtJsonSocketLib::DebugLog::ShowDebug( X, __LINE__, __FUNCTION__, __FILE__)

enum QtJsonSocketError
{
    None,
    Connect,
    SendSocketSize,
    SendSocketSource,
    SendTimeout,
    RecvTimeout,
    RecvSource,
    RecvSize,
    DecodeByteArrayToQJsonDocument,

};

class QtJsonSocketLib_v3 : public Qt_Json_Socket_Lib
{
    Q_OBJECT
public:
    QtJsonSocketLib_v3(QObject *parent = nullptr);

    QtJsonSocketLib_v3(bool TimeoutActive, bool EmitEvent = false);

    ~QtJsonSocketLib_v3();

    virtual bool make_QTcpSocket() override;

    virtual void set_socket(QTcpSocket* sock) override;

    virtual QTcpSocket *get_socket() override;

    virtual void set_defaulttimeout(int time);

    virtual void ShowDebugMessage(bool b);

    virtual bool send_QByteArray(QByteArray arr) override;

    virtual QByteArray recv_QByteArray() override;

    virtual bool send_Json(QJsonObject obj) override;

    virtual QJsonObject RecvJson();

    virtual QJsonDocument recv_Json() override;

    virtual void connect_server(const QString& Ip, qint64 port) override;

    virtual bool is_connect() override;

    virtual void disconnect_server() override;
    virtual void disconnect_socket() override;

    virtual void delete_QTcpSocket() override;

    virtual QString GetErrorString();
    virtual unsigned int ErrorType();

protected:
    void ErrorException(const QtJsonSocketError& e);

signals:
    void OnRecvEvent();

    void OnDisconnectEvent();

public slots:
    void OnRecvEventFromSocket();

    void OnDisconnectEventFromSocket();

protected:
    QSharedPointer<QTcpSocket> Socket;

    bool TimeoutActive;

    bool EmitEvent;

    unsigned int DefaultTimeout;

    QtJsonSocketError ErrType;

    QString ErrorString;
};

#endif // QTJSONSOCKETLIB_V3_H

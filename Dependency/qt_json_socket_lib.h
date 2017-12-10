#ifndef QT_JSON_SOCKET_LIB_H
#define QT_JSON_SOCKET_LIB_H

#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QString>
#include <QMetaMethod>

class Qt_Json_Socket_Lib: public QObject
{
    Q_OBJECT

public:

    Qt_Json_Socket_Lib();
    ~Qt_Json_Socket_Lib();

    //클라이언트 소켓 생성
    virtual bool make_QTcpSocket();

    //소켓
    virtual void set_socket(QTcpSocket* sock);

    //소켓 받기
    virtual QTcpSocket* get_socket();

    //QByteArray 전송
    virtual bool send_QByteArray(QByteArray arr);

    //QByteArray 수신
    virtual QByteArray recv_QByteArray();

    //Json전송
    virtual bool send_Json(QJsonObject obj);

    //Json수신
    virtual QJsonDocument recv_Json();

    //connect 타임아웃
    virtual void set_connect_timeout(qint64 t);

    //send 타임아웃 세팅
    virtual void set_send_timeout(qint64 t);

    //recv 타임아웃 세팅
    virtual void set_recv_timeout(qint64 t);

    //connect
    virtual void connect_server(const QString& a, qint64 port);

    //connect되었는지 확인
    virtual bool is_connect();

    //disconnect
    virtual void disconnect_server();
    virtual void disconnect_socket();

    //소켓디스크립터 삭제
    virtual void delete_QTcpSocket();


protected slots:

    //소켓으로부터 disconnect 시그널을 받아서 disconnect_from_socket()으로 emit함
    virtual void disconnect_slot();

Q_SIGNALS:

    //만약 disconnect_가 됬을 경우
    void disconnect_from_socket();


private:
    QTcpSocket*     sock__;
    QJsonDocument   doc__;
    QJsonObject     obj__;

    qint64          send_timeout__;
    qint64          recv_timeout__;
    qint64          connect_timeout__;

};

#endif // QT_JSON_SOCKET_LIB_H

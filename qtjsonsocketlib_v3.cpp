#include "qtjsonsocketlib_v3.h"

bool QtJsonSocketLib::DebugLog::ShowDebugMessage = false;

QtJsonSocketLib_v3::QtJsonSocketLib_v3(QObject *parent ) :Qt_Json_Socket_Lib(),
    DefaultTimeout(1000),
    EmitEvent(false),
    ErrType(QtJsonSocketError::None)
{

}

QtJsonSocketLib_v3::QtJsonSocketLib_v3(bool TimeoutActive, bool EmitEvent):TimeoutActive(TimeoutActive),
    DefaultTimeout(1000),
    EmitEvent(EmitEvent),
    ErrType(QtJsonSocketError::None)
{

}

QtJsonSocketLib_v3::~QtJsonSocketLib_v3()
{
    if(Socket.isNull() == false)
    {
        disconnect_socket();
    }

    Socket.clear();
}

bool QtJsonSocketLib_v3::make_QTcpSocket()
{
    Socket = QSharedPointer<QTcpSocket>::create();

    if(EmitEvent == true)
    {
        QObject::connect(Socket.data(), SIGNAL(readyRead()), this, SLOT(OnRecvEventFromSocket()));
    }

    QObject::connect(Socket.data(), SIGNAL(disconnected()), this, SLOT(OnDisconnectEventFromSocket()));

    return !Socket.isNull();
}

void QtJsonSocketLib_v3::set_socket(QTcpSocket *sock)
{
    if(Socket.isNull() == false)
    {
        if (Socket->isOpen() == true)
        {
            Socket->close();
        }
        Socket.clear();
    }

    Socket.reset(sock);

    /*
     * 소켓의 데이터가 들어왔다는 시그널을 바인드
     * */

    if(EmitEvent == true)
    {
        QObject::connect(Socket.data(), SIGNAL(readyRead()), this, SLOT(OnRecvEventFromSocket()));
    }

    QObject::connect(Socket.data(), SIGNAL(disconnected()), this, SLOT(OnDisconnectEventFromSocket()));
}

QTcpSocket *QtJsonSocketLib_v3::get_socket()
{
    return Socket.data();
}

void QtJsonSocketLib_v3::set_defaulttimeout(int time)
{
    DefaultTimeout = time;
}

void QtJsonSocketLib_v3::ShowDebugMessage(bool b)
{
    QtJsonSocketLib::DebugLog::ShowDebugMessage = b;
}

bool QtJsonSocketLib_v3::send_QByteArray(QByteArray arr)
{
    try
    {
        if(Socket->isOpen() == false)
        {
            throw QtJsonSocketError::Connect;
        }

        QByteArray SendBinaryToBase64 = arr.toBase64();

        qint64 ReesultSendByte = 0;
        qint64 ResultSendSource = 0;

        ReesultSendByte = Socket->write(QString::number(SendBinaryToBase64.size()).toUtf8(), sizeof(unsigned int));

        ResultSendSource = Socket->write(SendBinaryToBase64);

        if(ReesultSendByte == -1)
        {
            throw QtJsonSocketError::SendSocketSize;
        }

        if(ResultSendSource == -1)
        {
            throw QtJsonSocketError::SendSocketSource;
        }
    }

    catch(const QtJsonSocketError& e)
    {
        ErrorException(e);
        return false;
    }

    return true;
}

QByteArray QtJsonSocketLib_v3::recv_QByteArray()
{
    try
    {
        if(Socket->isOpen() == false)
        {
            throw QtJsonSocketError::Connect;
        }

        unsigned int RecvBinarySize = 0;

        QByteArray RecvSourceSize;
        QByteArray RecvSource;
        QByteArray ReturnByteArray;

        if(TimeoutActive == true && EmitEvent == false)
        {
            if( Socket->waitForReadyRead(DefaultTimeout) == false)
            {
                throw QtJsonSocketError::RecvTimeout;
            }
        }

        RecvSourceSize = Socket->read( sizeof(unsigned int));

        if(RecvSourceSize.isEmpty() == true)
        {
            throw QtJsonSocketError::RecvSize;
        }

        RecvBinarySize = static_cast<unsigned int>(RecvSourceSize.toInt());

        Socket->read(RecvBinarySize).isEmpty();

        RecvSource = Socket->readAll();

        if(RecvSource.isEmpty() == true)
        {
            throw QtJsonSocketError::RecvSource;
        }

        ReturnByteArray = QByteArray::fromBase64(RecvSource);

        return ReturnByteArray;
    }

    catch(const QtJsonSocketError& e)
    {
        ErrorException(e);
    }
}

bool QtJsonSocketLib_v3::send_Json(QJsonObject obj)
{
    QJsonDocument doc(obj);

    QByteArray SendJson= doc.toJson();

    return send_QByteArray(SendJson);
}

QJsonObject QtJsonSocketLib_v3::RecvJson()
{

    QJsonObject ReturnObject;

    QJsonDocument doc = recv_Json();

    if(doc.isNull() == true)
    {
        ErrorException(QtJsonSocketError::DecodeByteArrayToQJsonDocument);

        return ReturnObject;
    }

    ReturnObject = doc.object();

    return ReturnObject;
}

QJsonDocument QtJsonSocketLib_v3::recv_Json()
{
    QJsonDocument ReturnObject;

    QByteArray RecvJsonByteArray = recv_QByteArray();

    ReturnObject = QJsonDocument::fromJson(RecvJsonByteArray);

    return ReturnObject;
}

void QtJsonSocketLib_v3::connect_server(const QString &Ip, qint64 port)
{
    Socket->connectToHost(Ip, port);

    if (Socket->waitForConnected(DefaultTimeout) == false)
    {
        ErrorException(QtJsonSocketError::Connect);
    }
}

bool QtJsonSocketLib_v3::is_connect()
{
    if(Socket.isNull() == true)
    {
        return false;
    }

    bool Connected = (Socket->state() == QTcpSocket::ConnectedState);

    return Connected;
}

void QtJsonSocketLib_v3::disconnect_server()
{
    disconnect_socket();
}

void QtJsonSocketLib_v3::disconnect_socket()
{

    if(Socket.isNull() == false)
    {
        Socket->disconnectFromHost();
    }

    if(EmitEvent == true)
    {
        QObject::disconnect(Socket.data(), SIGNAL(readyRead()), this, SLOT(OnRecvEventFromSocket()));
    }

    QObject::disconnect(Socket.data(), SIGNAL(disconnected()), this, SLOT(OnDisconnectEventFromSocket()));

}

void QtJsonSocketLib_v3::delete_QTcpSocket()
{
    if(Socket.isNull())
    {
        return;
    }

    if(is_connect())
    {
        disconnect_socket();
    }

    Socket.clear();
    Socket = nullptr;
}

QString QtJsonSocketLib_v3::GetErrorString()
{
    return ErrorString;
}

unsigned int QtJsonSocketLib_v3::ErrorType()
{
    return ErrType;
}

void QtJsonSocketLib_v3::ErrorException(const QtJsonSocketError &e)
{
    switch(e)
    {
    case QtJsonSocketError::Connect:
        ErrType = QtJsonSocketError::Connect;
        ErrorString = "[Error] : Server is not Connect" + ErrorString;
        DEBUGLOG(Socket->errorString());
        break;

    case QtJsonSocketError::SendSocketSize:
        ErrType = QtJsonSocketError::SendSocketSize;
        ErrorString = "[Error] : Sned PayloadSize not Send" + ErrorString;
        DEBUGLOG(Socket->errorString());
        break;

    case QtJsonSocketError::SendSocketSource:
        ErrType = QtJsonSocketError::SendSocketSource;
        ErrorString = "[Error] : Sned Source is not Send" + ErrorString;
        DEBUGLOG(Socket->errorString());
        break;

    case QtJsonSocketError::SendTimeout:
        ErrType = QtJsonSocketError::SendTimeout;
        ErrorString = "[Error] : Send Timeout" + ErrorString;
        DEBUGLOG(Socket->errorString());
        break;

    case QtJsonSocketError::RecvTimeout:
        ErrType = QtJsonSocketError::RecvTimeout;
        ErrorString = "[Error] : Recv Timeout " + ErrorString;
        DEBUGLOG(Socket->errorString());
        break;

    case QtJsonSocketError::RecvSource:
        ErrType = QtJsonSocketError::RecvSource;
        ErrorString = "[Error] : RecvSource is not Recv" + ErrorString;
        DEBUGLOG(Socket->errorString());
        break;

    case QtJsonSocketError::RecvSize:
        ErrType = QtJsonSocketError::RecvSize;
        ErrorString = "[Error] : RecvSize is not Recv" + ErrorString;
        DEBUGLOG(Socket->errorString());
        break;
    }
}

void QtJsonSocketLib_v3::OnRecvEventFromSocket()
{
    emit OnRecvEvent();
}

void QtJsonSocketLib_v3::OnDisconnectEventFromSocket()
{
    emit OnDisconnectEvent();
}

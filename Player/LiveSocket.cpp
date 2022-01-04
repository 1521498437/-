#include "LiveSocket.h"
#include "Service.h"
#include <cstring>
#include <QHostAddress>
#include <QDebug>

#define self (*this)

bool LiveSocket::connectToServer(const QString& ip, int port)
{
   _sock.connectToHost(QHostAddress(ip), port);
   _conned = _sock.waitForConnected(1000); //阻塞
   return _conned;
}

LiveSocket::LiveSocket() : _sock(this)
{
   //connect(this, &QTcpSocket::connected, [this]() {
   //   //ui.tedtRecv.append(local("成功和服务器建立连接"));
   //});
#define BYTE_ARR QByteArray(bytes.data() + sizeof(srv::PackHead<void>), bytes.size() - sizeof(srv::PackHead<void>))

   connect(&_sock, &QTcpSocket::readyRead, [this]() {
      QByteArray bytes = _sock.readAll();
      if (!bytes.isNull() && !bytes.isEmpty())
      {
         auto head = (srv::PackHead<void>*)bytes.data();
         switch (head->ser_type)
         {
            case srv::ServiceId<srv::LoginRes>() : emit self.loginRes(BYTE_ARR); break;
            case srv::ServiceId<srv::RegistRes>() : emit self.registRes(BYTE_ARR); break;
            case srv::ServiceId<srv::StartLiveRes>() : emit self.startLiveRes(BYTE_ARR); break;
            case srv::ServiceId<srv::LivingUserRec>() : emit self.livingUserRec(BYTE_ARR, head->body_count); break;
            case srv::ServiceId<srv::RoomInfoRec>() : emit self.roomInfoRec(BYTE_ARR, head->body_count); break;
            case srv::ServiceId<srv::MessageReq>() : emit self.message(BYTE_ARR); break;
            default: break;
         }
      }
   });
}

LiveSocket::~LiveSocket()
{
   if (_conned)
   {
      _sock.close();
      _sock.disconnectFromHost();
      _sock.waitForDisconnected(1000);
   }
}

//发送数据包模板函数
template <typename T>
static bool sendPack(QTcpSocket& sock, const srv::PackHead<T>& head, const T& body)
{
   sock.write((const char*)&head, sizeof(srv::PackHead<T>));
   sock.write((const char*)&body, sizeof(T));
   return sock.waitForBytesWritten(1000);
}

void LiveSocket::loginReq(const QString& uname, const QString& upass)
{
   srv::LoginReq req;
   snprintf(req.user, sizeof(req.user), "%s", uname.toStdString().c_str());
   snprintf(req.pass, sizeof(req.pass), "%s", upass.toStdString().c_str());
   sendPack(_sock, srv::PackHead<decltype(req)>(), req);
}

void LiveSocket::registReq(const QString& uname, const QString& upass)
{
   srv::RegistReq req;
   snprintf(req.user, sizeof(req.user), "%s", uname.toStdString().c_str());
   snprintf(req.pass, sizeof(req.pass), "%s", upass.toStdString().c_str());
   sendPack(_sock, srv::PackHead<decltype(req)>(), req);
}

void LiveSocket::startLiveReq(const QString& user, const QString& url)
{
   srv::StartLiveReq req;
   snprintf(req.user, sizeof(req.user), "%s", user.toStdString().c_str());
   snprintf(req.url, sizeof(req.url), "%s", url.toStdString().c_str());
   sendPack(_sock, srv::PackHead<decltype(req)>(), req);
}

void LiveSocket::watchLiveReq(const QString& streamer, const QString& audience)
{
   srv::WatchLiveReq req;
   snprintf(req.streamer, sizeof(req.streamer), "%s", streamer.toStdString().c_str());
   snprintf(req.audience, sizeof(req.audience), "%s", audience.toStdString().c_str());
   sendPack(_sock, srv::PackHead<decltype(req)>(), req);
}

void LiveSocket::message(const QString& user, const QString& msg)
{
   srv::MessageReq req;
   snprintf(req.user, sizeof(req.user), "%s", user.toStdString().c_str());
   snprintf(req.msg, sizeof(req.msg), "%s", msg.toStdString().c_str());
   sendPack(_sock, srv::PackHead<decltype(req)>(), req);
}

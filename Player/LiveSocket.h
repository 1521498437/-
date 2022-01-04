#pragma once
#include <atomic>
#include <QString>
#include <QTcpSocket>

class LiveSocket : public QObject
{
	Q_OBJECT

public:
	static LiveSocket& Get() {
		static LiveSocket instance;
		return instance;
	}
	bool connectToServer(const QString& ip, int port);
	bool isConnected() const { return _conned; };

public:
	void loginReq(const QString& uname, const QString& upass);
	void registReq(const QString& uname, const QString& upass);
	void startLiveReq(const QString& user, const QString& url);
	void watchLiveReq(const QString& streamer, const QString& audience);
	void message(const QString& user, const QString& msg);

signals:
	void loginRes(QByteArray);
	void registRes(QByteArray);
	void startLiveRes(QByteArray);
	void livingUserRec(QByteArray, int);
	void roomInfoRec(QByteArray, int);
	void message(QByteArray);

protected:
	LiveSocket();
	~LiveSocket();

protected:
	QTcpSocket _sock;
	std::atomic_bool _conned{ false };
};


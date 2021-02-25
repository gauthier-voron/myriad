#include <cstdio>

#include <map>
#include <memory>

#include <metasys/net/InetAddress.hxx>
#include <metasys/net/TcpSocket.hxx>

#include <myriad/net/concepts.hxx>
#include <myriad/net/TcpThreadNetwork.hxx>


using metasys::InetAddress;
using metasys::TcpSocket;
using myriad::Connection;
using myriad::TcpThreadNetwork;
using std::map;


class MyNetwork;

class MyRecvHandler
{
	InetAddress _addr;


public:
	MyRecvHandler() = delete;

	MyRecvHandler(const InetAddress &addr) noexcept
		: _addr(addr)
	{
	}

	constexpr MyRecvHandler(const MyRecvHandler &other) noexcept = default;
	MyRecvHandler(MyRecvHandler &&other) = delete;

	MyRecvHandler &operator=(const MyRecvHandler &other) noexcept=default;
	MyRecvHandler &operator=(MyRecvHandler &&other) = delete;

	void onReceive()
	{
		printf("receive from %hhu.%hhu.%hhu.%hhu:%hu\n", _addr.ip()[0],
		       _addr.ip()[1], _addr.ip()[2], _addr.ip()[3],
		       _addr.port());
	}

	void onClose(int err)
	{
		printf("recv handler for %hhu.%hhu.%hhu.%hhu:%hu closes with "
		       "%d\n", _addr.ip()[0], _addr.ip()[1], _addr.ip()[2],
		       _addr.ip()[3], _addr.port(), err);
	}
};

template<typename Network>
struct MyHandler
{
	int id;
	int info;
	map<InetAddress, typename Network::Connection> conns;

	MyHandler() = delete;

	constexpr MyHandler(int _id, int _info) noexcept
		: id(_id), info(_info)
	{
	}

	MyHandler(const MyHandler &) = delete;
	MyHandler(MyHandler &&other) = default;


	MyHandler &operator=(const MyHandler &) = delete;
	MyHandler &operator=(MyHandler &&other) = default;


	// template<typename C>
	// // requires Connection<C, MyRecvHandler>
	// void onAccept(C &&conn, const InetAddress &from)
	void onAccept(Network::Connection &&conn, const InetAddress &from)
	{
		conns.emplace(from, std::move(conn));
		debug("%d accepts from %hu\n", id, from.port());
		conn.receive(from);
		debug("accepted\n");
	}

	void onReceive()
	{
		debug("receive\n");
	}

	void onClose(int err)
	{
		debug("close %d on %d\n", id, err);
	}
};


class MyInstHandler;

class MyNetwork : public TcpThreadNetwork<MyInstHandler, MyRecvHandler>
{
};

class MyInstHandler : public MyHandler<MyNetwork>
{
 public:
	using MyHandler::MyHandler;
};



int main()
{
	MyNetwork net;

	net.listen(InetAddress(8000), MyInstHandler(0, 8000));

	// // ::usleep(300000);

	// // net.listen(InetAddress(8001), MyHandler(1, 8001));

	// // ::usleep(300000);

	// // net.listen(InetAddress(8000), MyHandler(2, 8000));

	// // ::usleep(100000);

	TcpSocket::instance(InetAddress::localhost(8000));

	// ::usleep(500000);
	::sleep(2);

	return 0;
}

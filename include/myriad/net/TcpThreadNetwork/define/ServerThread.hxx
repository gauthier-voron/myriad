#ifndef _INCLUDE_MYRIAD_NET_TCPTHREADNETWORK_DEFINE_SERVERTHREAD_HXX_
#define _INCLUDE_MYRIAD_NET_TCPTHREADNETWORK_DEFINE_SERVERTHREAD_HXX_


#include <utility>

#include <metasys/net/InetAddress.hxx>
#include <metasys/net/TcpServerSocket.hxx>
#include <metasys/net/TcpSocket.hxx>
#include <metasys/sched/Thread.hxx>

#include <myriad/bind.hxx>
#include <myriad/bind/Handler.hxx>
#include <myriad/net/TcpThreadNetwork/declare/ServerThread.hxx>


namespace myriad {


namespace detail {


namespace TcpThreadNetwork {


template<typename Parent, typename Accepter>
class ServerThread
{
	static_assert (AcceptHandler<Accepter, typename Parent::Connection>);


	using Conn = typename Parent::Connection;
	using InetAddress = metasys::InetAddress;
	using Jthread = metasys::Jthread;
	using ServerSocket = metasys::TcpServerSocket;
	using Socket = metasys::TcpSocket;


	Handler<Accepter>  _accepter;
	ServerSocket       _socket;
	Jthread            _thread;


 public:
	template<typename ... Args>
	ServerThread(const InetAddress &addr, int backlog, Args && ... args)
		: _accepter(std::in_place, std::forward<Args>(args) ...)
		, _socket(ServerSocket::instance(addr, backlog))
		, _thread(Jthread::Constexpr<&ServerThread::run>(), this)
	{
	}

	ServerThread(const ServerThread &) = delete;
	ServerThread(ServerThread &&other) noexcept = default;


	void handleAccept(Socket &&sock, const InetAddress &addr) noexcept
	{
		value(*_accepter.load()).onAccept(Parent::buildConnection
						  (std::move(sock)), addr);
	}

	void run() {
		InetAddress from;
		Socket sock;

		try {
			while (true) {
				sock = _socket.accept(&from);
				handleAccept(std::move(sock), from);
			}
		} catch (...) {
			// TODO: notify parent somehow
			value(*_accepter.load()).onClose(0);
			throw;
		}
	}

	template<typename ... Args>
	void replaceCallback(Args && ... args)
	{
		_accepter.store(std::forward<Args>(args) ...);
	}
};


}


}


}


#endif

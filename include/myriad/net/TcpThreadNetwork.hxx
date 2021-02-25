#ifndef _INCLUDE_MYRIAD_NET_TCPTHREADNETWORK_HXX_
#define _INCLUDE_MYRIAD_NET_TCPTHREADNETWORK_HXX_


#include <atomic>
#include <concepts>
#include <map>
#include <type_traits>

#include <metasys/net/InetAddress.hxx>
#include <metasys/net/TcpServerSocket.hxx>

#include <myriad/bind.hxx>
#include <myriad/net/TcpThreadNetwork/declare/Connection.hxx>
#include <myriad/net/TcpThreadNetwork/define/ServerThread.hxx>


namespace myriad {


template<typename Accepter, typename Receiver>
class TcpThreadNetwork
{
	using This = TcpThreadNetwork
		<Accepter, Receiver>;

	using InetAddress = metasys::InetAddress;

	using ServerThread = detail::TcpThreadNetwork::ServerThread
		<This, Accepter>;


 public:
	using Connection = detail::TcpThreadNetwork::Connection
		<This, Receiver>;


 private:
	int                                  _serverBacklog = 32;
	std::map<InetAddress, ServerThread>  _serverThreads;


	friend ServerThread;
	template<typename ... Args>
	static Connection buildConnection(Args && ... args)
	{
		return Connection(std::forward<Args>(args) ...);
	}


 public:
	template<typename _Accepter>
	void subscribeAcceptedConnection(const InetAddress &addr,
					 _Accepter &&cb)
	{
		auto pair = _serverThreads.try_emplace
			(addr, addr, _serverBacklog,
			 std::forward<_Accepter>(cb));

		if (pair.second)  // new thread
			return;

		pair.first->second.replaceCallback
			(std::forward<_Accepter>(cb));
	}

	template<typename _Accepter>
	void listen(const InetAddress &addr, _Accepter &&cb)
	{
		subscribeAcceptedConnection(addr, std::forward<_Accepter>(cb));
	}
};


}


#include <myriad/net/TcpThreadNetwork/define/Connection.hxx>
#include <myriad/net/TcpThreadNetwork/define/ReadThread.hxx>


#endif

#ifndef _INCLUDE_MYRIAD_NET_TCPTHREADNETWORK_DECLARE_SERVERTHREAD_HXX_
#define _INCLUDE_MYRIAD_NET_TCPTHREADNETWORK_DECLARE_SERVERTHREAD_HXX_


#include <concepts>
#include <utility>

#include <metasys/net/InetAddress.hxx>


namespace myriad {


namespace detail {


namespace TcpThreadNetwork {


template<typename T, typename Connection>
concept AcceptHandler = requires (T a, int err, Connection &&conn,
				  const metasys::InetAddress &addr)
{
	{ value(a).onAccept(std::move(conn), addr) } -> std::same_as<void>;
	{ value(a).onClose(errno) } -> std::same_as<void>;
};


template<typename Parent, typename Accepter>
class ServerThread;


}


}


}


#endif

#ifndef _INCLUDE_MYRIAD_NET_TCPTHREADNETWORK_DECLARE_CONNECTION_HXX_
#define _INCLUDE_MYRIAD_NET_TCPTHREADNETWORK_DECLARE_CONNECTION_HXX_


#include <concepts>

#include <myriad/bind.hxx>


namespace myriad {


namespace detail {


namespace TcpThreadNetwork {


template<typename T>
concept ReceiveHandler = requires (T a, int err)
{
	{ myriad::value(a).onReceive() } -> std::same_as<void>;
	{ myriad::value(a).onClose(err) } -> std::same_as<void>;
};


template<typename Parent, typename Receiver>
class Connection;


}


}


}


#endif

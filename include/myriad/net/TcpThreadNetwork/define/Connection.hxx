#ifndef _INCLUDE_MYRIAD_NET_TCPTHREADNETWORK_DEFINE_CONNECTION_HXX_
#define _INCLUDE_MYRIAD_NET_TCPTHREADNETWORK_DEFINE_CONNECTION_HXX_


#include <optional>
#include <utility>

#include <metasys/net/TcpSocket.hxx>

#include <myriad/net/TcpThreadNetwork/declare/Connection.hxx>
#include <myriad/net/TcpThreadNetwork/define/ReadThread.hxx>


namespace myriad {


namespace detail {


namespace TcpThreadNetwork {


template<typename Parent, typename Receiver>
class Connection
{
	static_assert (ReceiveHandler<Receiver>);


	using This = Connection<Parent, Receiver>;
	using Reader = ReadThread<This, Receiver>;
	using Socket = metasys::TcpSocket;


	friend Parent;
	friend Reader;


	Socket                 _socket;
	std::optional<Reader>  _reader;


	constexpr Connection(Socket &&socket) noexcept
		: _socket(std::move(socket))
	{
	}

	Socket *socket() noexcept
	{
		return &_socket;
	}


 public:
	Connection(const Connection &) = delete;
	Connection(Connection &&other) = default;

	Connection &operator=(const Connection &) = delete;
	Connection &operator=(Connection &&other) = default;

	template<typename ... Args>
	void receive(Args && ... args)
	{
		if (_reader) {
			_reader->replaceCallback(std::forward<Args>(args) ...);
		} else {
			_reader.emplace(this, std::forward<Args>(args) ...);
		}
	}

	void close()
	{
		_socket.close();
	}
};


}


}


}



#endif

#ifndef _INCLUDE_MYRIAD_NET_TCPTHREADNETWORK_DEFINE_READTHREAD_HXX_
#define _INCLUDE_MYRIAD_NET_TCPTHREADNETWORK_DEFINE_READTHREAD_HXX_


#include <atomic>
#include <utility>

#include <metasys/net/TcpSocket.hxx>
#include <metasys/sched/Thread.hxx>

#include <myriad/bind.hxx>
#include <myriad/bind/Handler.hxx>


namespace myriad {


namespace detail {


namespace TcpThreadNetwork {


template<typename Parent, typename Receiver>
class ReadThread
{
	using Jthread = metasys::Jthread;
	using Socket = metasys::TcpSocket;


	Parent             *_parent;
	Handler<Receiver>   _receiver;
	std::atomic<bool>   _stopping = false;
	Jthread             _thread;


	void _handleReceive()
	{
		try {
			value(*_receiver.load()).onReceive();
		} catch (...) {
			if (_stopping)
				throw;
		}
	}


 public:
	template<typename ... Args>
	ReadThread(Parent *parent, Args && ... args)
		: _parent(parent)
		, _receiver(std::in_place, std::forward<Args>(args) ...)
		, _thread(Jthread::Constexpr<&ReadThread::run>(), this)
	{
	}

	ReadThread(const ReadThread &) = delete;

	ReadThread(ReadThread &&other) noexcept
		: _parent(other._parent)
		, _receiver(other._receiver)
		, _thread(std::move(other._thread))
	{
	}

	~ReadThread()
	{
		_stopping = true;
	}

	ReadThread &operator=(const ReadThread &) = delete;

	ReadThread &operator=(ReadThread &&other) noexcept
	{
		_parent = other._parent;
		_receiver = other._receiver.load();
		_thread = std::move(other._thread);

		return *this;
	}

	void run()
	{
		try {
			while (true) {
				_handleReceive();
				::sleep(1);
			}
		} catch (...) {
			assert(_stopping);
			// TODO: notify parent somehow
			value(*_receiver.load()).onClose(0);
			throw;
		}
	}

	template<typename ... Args>
	void replaceCallback(Args && ... args)
	{
		_receiver.store(std::forward<Args>(args) ...);
	}
};


}


}


}


#endif

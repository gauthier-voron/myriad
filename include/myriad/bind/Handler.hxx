#ifndef _INCLUDE_MYRIAD_BIND_HANDLER_HXX_
#define _INCLUDE_MYRIAD_BIND_HANDLER_HXX_


#include <atomic>
#include <concepts>
#include <utility>

#include <myriad/sched/AtomicPtr.hxx>
#include <myriad/sched/AtomicTrivial.hxx>


namespace myriad {


template<typename T>
class Handler;


template<typename T>
requires AtomicTrivial<T>
class Handler<T>
{
	std::atomic<T>  _inner;


 public:
	explicit constexpr Handler(const T &inner) noexcept
		: _inner(inner)
	{
	}

	template<typename ... Args>
	requires std::is_constructible_v<T, Args ...>
	explicit constexpr Handler(std::in_place_t, Args && ... args) noexcept
		: _inner(T(std::forward<Args>(args) ...))
	{
	}

	constexpr Handler(const Handler &other) noexcept = default;
	Handler(Handler &&) = delete;

	Handler &operator=(const Handler &other) noexcept = default;
	Handler &operator=(Handler &&) = delete;

	Handler &operator=(const T &inner) noexcept
	{
		store(inner);
		return *this;
	}

	void store(const T &inner) noexcept
	{
		_inner.store(inner);
	}

	template<typename ... Args>
	requires std::is_constructible_v<T, Args ...>
	void store(Args && ... args) noexcept
	{
		_inner.store(T(std::forward<Args>(args) ...));
	}


	class Reference
	{
		T  _inner;


		friend Handler;


		constexpr Reference(const T &inner) noexcept
			: _inner(inner)
		{
		}


	 public:
		Reference(const Reference &) = delete;
		Reference(Reference &&) = delete;

		Reference &operator=(const Reference &) = delete;
		Reference &operator=(Reference &&) = delete;


		T &operator*() noexcept
		{
			return _inner;
		}

		const T &operator*() const noexcept
		{
			return _inner;
		}

		T *operator->() noexcept
		{
			return &_inner;
		}

		const T *operator->() const noexcept
		{
			return &_inner;
		}
	};

	constexpr Reference load() noexcept
	{
		return Reference(_inner.load());
	}
};


template<typename T>
requires (!AtomicTrivial<T>)
class Handler<T>
{
	AtomicPtr<T>  _inner;


 public:
	template<typename ... Args>
	constexpr Handler(std::in_place_t, Args && ... args) noexcept
		: _inner(std::forward<Args>(args) ...)
	{
	}

	constexpr Handler(const Handler &other) noexcept = default;
	Handler(Handler &&) = delete;

	Handler &operator=(const Handler &other) noexcept = default;
	Handler &operator=(Handler &&) = delete;

	template<typename Other>
	Handler &operator=(Other &&other)
	{
		store(std::forward<Other>(other));
		return *this;
	}

	template<typename Other>
	void store(Other &&other)
	{
		_inner.emplace(std::forward<Other>(other));
	}

	template<typename ... Args>
	void store(Args && ... args)
	{
		_inner.emplace(std::forward<Args>(args) ...);
	}


	AtomicPtr<T>::Reference load()
	{
		return _inner.load();
	}
};


}


#endif

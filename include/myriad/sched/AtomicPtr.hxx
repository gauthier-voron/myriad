#ifndef _INCLUDE_MYRIAD_SCHED_ATOMICPTR_HXX_
#define _INCLUDE_MYRIAD_SCHED_ATOMICPTR_HXX_


#include <atomic>
#include <utility>


namespace myriad {


template<typename T>
class AtomicPtr
{
	struct Value
	{
		std::atomic<size_t>  _refcnt = 1;
		T                    _inner;


	 public:
		template<typename ... Args>
		Value(Args && ... args)
			: _inner(std::forward<Args>(args) ...)
		{
		}

		Value &retain() noexcept
		{
			_refcnt++;
			return *this;
		}

		[[nodiscard ("must be freed if return true")]]
		bool release() noexcept
		{
			return (_refcnt.fetch_sub(1) == 1);
		}

		T &inner() noexcept
		{
			return _inner;
		}

		const T &inner() const noexcept
		{
			return _inner;
		}
	};


	std::atomic<Value *>  _aptr;


 public:
	template<typename ... Args>
	AtomicPtr(Args && ... args)
		: _aptr(new Value(std::forward<Args>(args) ...))
	{
	}

	AtomicPtr(const AtomicPtr &other) noexcept
		: _aptr(&other._aptr.load()->retain())
	{
	}

	AtomicPtr(AtomicPtr &&) = delete;

	~AtomicPtr()
	{
		Value *cpy = _aptr.load();

		if (cpy->release())
			delete cpy;
	}

	AtomicPtr &operator=(const AtomicPtr &other) noexcept
	{
		Value *nptr = other._aptr.load();
		Value *old = _aptr.exchange(nptr);

		nptr->retain();

		if (old->release())
			delete old;

		return *this;
	}

	AtomicPtr &operator=(AtomicPtr &&) = delete;

	template<typename ... Args>
	void emplace(Args && ... args)
	{
		Value *nptr = new Value(std::forward<Args>(args) ...);
		Value *old = _aptr.exchange(nptr);

		if (old->release())
			delete old;
	}

	class Reference
	{
		friend AtomicPtr;


		Value  &_ptr;


		[[gnu::always_inline]]
		explicit constexpr Reference(Value *ptr) noexcept
			: _ptr(ptr->retain())
		{
		}


	 public:
		Reference(const Reference &) = delete;
		Reference(Reference &&) = delete;

		[[gnu::always_inline]] ~Reference()
		{
			if (_ptr.release()) [[unlikely]]
				delete &_ptr;
		}

		Reference &operator=(const Reference &) = delete;
		Reference &operator=(Reference &&) = delete;

		T &operator*() noexcept
		{
			return _ptr.inner();
		}

		const T &operator*() const noexcept
		{
			return _ptr.inner();
		}

		T *operator->() noexcept
		{
			return &_ptr.inner();
		}

		const T *operator->() const noexcept
		{
			return &_ptr.inner();
		}
	};

	[[gnu::always_inline]] Reference load() noexcept
	{
		return Reference(_aptr.load());
	}
};


}


#endif

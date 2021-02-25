#ifndef _INCLUDE_MYRIAD_SCHED_ATOMICTRIVIAL_HXX_
#define _INCLUDE_MYRIAD_SCHED_ATOMICTRIVIAL_HXX_


#include <atomic>
#include <concepts>


namespace myriad {


template<typename T>
concept AtomicTrivial =
	std::is_trivially_copyable_v<T> &&
	std::atomic<T>::is_always_lock_free;


}


#endif

#ifndef _INCLUDE_MYRIAD_LOG_HXX_
#define _INCLUDE_MYRIAD_LOG_HXX_


#include <cstdio>

#include <utility>


// namespace myriad {


template<typename ... Args>
static inline void debug(Args && ... args)
{
	fprintf(stderr, std::forward<Args>(args) ...);
	fflush(stderr);
}


// }


#endif

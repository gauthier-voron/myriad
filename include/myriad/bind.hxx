#ifndef _INCLUDE_MYRIAD_BIND_HXX_
#define _INCLUDE_MYRIAD_BIND_HXX_


namespace myriad {


template<typename T>
T value(T &&a) noexcept
{
	return a;
}

template<typename T>
T &value(T &a) noexcept
{
	return a;
}

template<typename T>
T &value(T *a) noexcept
{
	return *a;
}


}


#endif

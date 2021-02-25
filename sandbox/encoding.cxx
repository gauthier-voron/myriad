#include <cstdio>

#include <concepts>
#include <string>


template<typename ... Ts>
class Decoder;


struct TypeWildcard
{
	template<typename T>
	operator T() const noexcept;
};



// template<size_t N>
// struct ArgCounter
// {
// 	static constexpr size_t value = N;
// };


template<size_t X>
struct Factorial
{
	static constexpr size_t value = X * Factorial<X-1>::value;
};

template<>
struct Factorial<0>
{
	static constexpr size_t value = 1;
};


template<size_t X, size_t Acc, size_t Y>
struct _InvFactorial;

template<size_t X, size_t Acc, size_t Y>
requires (Acc >= Y)
struct _InvFactorial<X, Acc, Y>
{
	static constexpr size_t value = X;
};

template<size_t X, size_t Acc, size_t Y>
requires (Acc < Y)
struct _InvFactorial<X, Acc, Y>
{
	static constexpr size_t value = _InvFactorial<X+1,Acc*(X+1),Y>::value;
};

template<size_t Y>
struct InvFactorial
{
	static constexpr size_t value = _InvFactorial<0,1,Y>::value;
};


template<auto Callable, typename ... Ts>
struct ArgCounter;

template<auto Callable, typename ... Ts>
requires (std::invocable<decltype (Callable), Ts ...>)
struct ArgCounter<Callable, Ts ...>
{
	static constexpr size_t value = sizeof ... (Ts);
};

template<auto Callable, typename ... Ts>
requires (!std::invocable<decltype (Callable), Ts ...>)
struct ArgCounter<Callable, Ts ...>
{
	static constexpr size_t value =
		ArgCounter<Callable, TypeWildcard, Ts ...>::value;
};




static_assert (Factorial<5>::value == 120);
static_assert (InvFactorial<120>::value == 5);
static_assert (InvFactorial<121>::value == 6);


void freceive(const std::string &s, double d)
{
}

void greceive(int x)
{
}


static_assert (std::invocable<decltype (freceive), const std::string, double>);
static_assert (std::invocable<decltype (freceive), TypeWildcard,TypeWildcard>);
static_assert (ArgCounter<freceive>::value == 2);
static_assert (ArgCounter<greceive>::value == 1);




class MySystem
{
 public:
	void receive(bool, int, const std::string &)
	{
	}

	void receive(int, int)
	{
	}
};



template<typename T, typename ... Args>
concept Receiver = requires (T a, Args ... args)
{
	a.receive(args ...);
};


static_assert (Receiver<MySystem, bool, int, std::string>);
static_assert (Receiver<MySystem, int, int>);


template<typename Handler, typename ... Ts>
struct ArgsCounter;

template<typename Handler, typename ... Ts>
requires (Receiver<Handler, Ts ...>)
struct ArgsCounter<Handler, Ts ...>
{
	static constexpr size_t value = sizeof ... (Ts);
};

template<typename Handler, typename ... Ts>
requires (!Receiver<Handler, Ts ...> && (sizeof ... (Ts) <= 16))
struct ArgsCounter<Handler, Ts ...>
{
	static constexpr size_t value =
		ArgsCounter<Handler, TypeWildcard, Ts ...>::value;
};


static_assert (ArgsCounter<MySystem>::value == 2);



void simplefunc(int)
{
}


#include <tuple>

template<typename ... Args>
constexpr std::tuple<Args...> yo(void (Args ...));


using proto = decltype (yo(simplefunc));


static_assert (std::same_as<proto, std::tuple<int>>);


template<auto Callable>
struct CallSignature
{
	template<typename Ret, typename ... Args>
	static constexpr Ret _extractRet(Ret (Args...));

	using Return = decltype (_extractRet(Callable));


	template<typename Ret, typename ... Args>
	static constexpr std::tuple<Args...> _extractArgs(Ret (Args...));

	using Arguments = decltype (_extractArgs(Callable));
};


static_assert (std::same_as<CallSignature<simplefunc>::Arguments,
	       std::tuple<int>>);



template<typename ... Ts>
struct SignatureSize;

template<>
struct SignatureSize<>
{
	static constexpr size_t value = 0;
};

template<typename T, typename ... Ts>
struct SignatureSize<T, Ts...>
{
	static constexpr size_t value = sizeof (T)
		+ SignatureSize<Ts...>::value;
};



static_assert (SignatureSize<>::value == 0);
static_assert (SignatureSize<int>::value == 4);
static_assert (SignatureSize<int, bool>::value == 5);
static_assert (SignatureSize<int, double, double, char>::value == 21);


template<auto F>
struct FunctionSignatureSize
{
	template<typename Ret, typename ... Args>
	static constexpr SignatureSize<Args...> _computeSize(Ret (Args...));

	static constexpr size_t value = decltype (_computeSize(F))::value;
};


static_assert (FunctionSignatureSize<simplefunc>::value == 4);


void ovfunc(int) { printf("1a\n"); }
void ovfunc(int, int) { printf("2a\n"); }

template<typename U, typename T>
requires (std::is_convertible_v<U, T>)
auto resolve(void (*func)(T), U)
{
	return func;
}

static_assert (std::same_as<decltype (resolve(&ovfunc, std::declval<TypeWildcard>())), void (*)(int)>);



void ovfunc2(int) {}
void ovfunc2(void *) {}

template<typename ... Ts>
struct TypeSize;

template<>
struct TypeSize<>
{
	static constexpr size_t value = 0;
};

template<typename T, typename ... Ts>
struct TypeSize<T, Ts ...>
{
	static constexpr size_t value = sizeof (T) + TypeSize<Ts ...>::value;
};

static_assert (TypeSize<int>::value == 4);
static_assert (TypeSize<void *>::value == 8);



struct MyOvSystem
{
	void receive(int) {}
	void receive(void *) {}
};

template<size_t S>
struct TypeSizeEqWildcard
{
	template<typename T>
	requires (sizeof (T) == S)
	constexpr operator T() const noexcept;
};

template<size_t S>
struct TypeSizeLeWildcard
{
	template<typename T>
	requires (sizeof (T) <= S)
	constexpr operator T() const noexcept;
};

template<size_t S>
struct TypeSizeGeWildcard
{
	template<typename T>
	requires (sizeof (T) >= S)
	constexpr operator T() const noexcept;
};

static_assert (Receiver<MyOvSystem, TypeSizeEqWildcard<8>>);
static_assert (Receiver<MyOvSystem, TypeSizeEqWildcard<4>>);
static_assert (Receiver<MyOvSystem, TypeSizeEqWildcard<1>> == false);

static_assert (Receiver<MyOvSystem, TypeSizeLeWildcard<8>> == false);
static_assert (Receiver<MyOvSystem, TypeSizeLeWildcard<4>>);
static_assert (Receiver<MyOvSystem, TypeSizeLeWildcard<1>> == false);

static_assert (Receiver<MyOvSystem, TypeSizeGeWildcard<8>>);
static_assert (Receiver<MyOvSystem, TypeSizeGeWildcard<4>> == false);
static_assert (Receiver<MyOvSystem, TypeSizeGeWildcard<1>> == false);


template<typename T>
struct Yo
{
	static constexpr T instance() noexcept;
};

using test = decltype (ovfunc(Yo::instance()));


int main()
{
	return 0;
}

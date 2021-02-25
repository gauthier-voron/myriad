#include <cstdio>

#include <string>
#include <utility>


#define newtype(new_name, old_name, ...)				\
	class new_name : public old_name				\
	__VA_OPT__(<) __VA_ARGS__ __VA_OPT__(>) {			\
	public:								\
		using old_name						\
			__VA_OPT__(<) __VA_ARGS__ __VA_OPT__(>)		\
			::old_name;					\
									\
		template<typename _NewTypeT>				\
		new_name &operator=(_NewTypeT && x)			\
		{							\
			old_name					\
				__VA_OPT__(<) __VA_ARGS__ __VA_OPT__(>)	\
				::operator=(std::forward<_NewTypeT>(x)); \
			return *this;					\
		}							\
	}


class A
{
 public:
	A() { printf("A()\n"); };
	explicit A(int n) { printf("A(int)\n"); }
	A(const A &other) { printf("A(const A &)\n"); };
	A(A &&other) { printf("A(A &&)\n"); };
	~A() { printf("~A();\n"); }

	A &operator=(const A &other) { printf("A=(const A &)\n");return *this;}
	A &operator=(A &&other) { printf("A=(A &&)\n"); return *this; }
	A &operator=(int n) { printf("A=(int)\n"); return *this; }
};

class B
{
 public:
	explicit B(int n) { printf("B(int)\n"); }
	B(const B &other) = delete;
	B(B &&other) { printf("B(B &&)\n"); };
	~B() { printf("~B();\n"); }

	B &operator=(const B &other) = delete;
	B &operator=(B &&other) { printf("B=(B &&)\n"); return *this; }
	B &operator=(int n) { printf("B=(int)\n"); return *this; }
};

template<typename T>
class C
{
	T  _x;

 public:
	explicit C(const T &x) : _x(x) { printf("C(const T &)\n"); }
};

template<typename T0, typename T1>
class D
{
	T0  _x0;
	T1  _x1;

 public:
	D(const T0 &x0, const T1 &x1)
		: _x0(x0), _x1(x1)
	{ printf("D(const T0 &, const T1 &)\n"); }
};


newtype(MyA, A);
newtype(MyB, B);
newtype(MyCi, C, int);
newtype(MyCs, C, std::string);

template<typename T>
newtype(MyDi, D, T, int);


int main()
{
	{
		A a0;
		A a1 { a0 };
		A a2 { std::move(a0) };
		A a3 { 12 };

		a0 = a2;
		a1 = std::move(a2);
		a2 = 17;
	}

	printf("\n");

	{
		MyA a0;
		MyA a1 { a0 };
		MyA a2 { std::move(a0) };
		MyA a3 { 12 };

		a0 = a2;
		a1 = std::move(a2);
		a2 = 17;
	}

	printf("\n");

	{
		B b0 { 12 };
		B b1 { std::move(b0) };
		// B b2 { b0 };
		// B b3;

		// b1 = b0;
		b0 = std::move(b1);
		b1 = 17;
	}

	printf("\n");

	{
		MyB b0 { 12 };
		MyB b1 { std::move(b0) };
		// MyB b2 { b0 };
		// MyB b3;

		// b1 = b0;
		b0 = std::move(b1);
		b1 = 17;
	}

	printf("\n");

	{
		MyCi c0 { 18 };
		MyCs c1 { "hello" };
	}

	printf("\n");

	{
		MyDi<bool> d0 { true, 177 };
	}

	return 0;
}

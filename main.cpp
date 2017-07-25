#include <memory>
#include <string>
#include <iostream>

struct end_chain {};

const end_chain END;

template <typename T>
struct executor
{
	T ret{};
	bool fail = true;

	executor(T v) : fail(!v), ret(std::move(v)) {}
	executor() {}

	template <typename Arg0 >
	auto operator , (Arg0&& fn)
	{
		using RET = decltype(fn(std::forward<T>(ret)));

		if (fail)
		{
			return executor<RET>{};
		}

		auto val = fn(std::forward<T>(ret));
		if (!val)
		{
			return executor<RET>{};
		}

		return executor<RET>{std::move(val)};
	}

	auto operator , (end_chain)
	{
		if (fail)
		{
			return T{};
		}

		return std::move(ret);
	}
};

template <typename T>
auto start_chain(T&& v)
{
	return executor<T>(std::move(v));
}

struct node
{
	std::unique_ptr<node> next;
	char data = 'A';
};

int main(int argc, char* argv[])
{
	auto upit = [](int i) -> int
	{
		return ++i;
	};

	auto failit = [](auto i)
	{
		return 0;
	};

	auto val = (start_chain(100), upit, upit, failit, END);

	std::cout << val << std::endl;

	node root;

	root.next = std::make_unique<node>();
	root.next->next = std::make_unique<node>();
	root.next->next->data = 'Z';

	auto getnext = [](auto p) { return p->next.get(); };
	auto getdata = [](auto p) { return p->data;  };

	auto val2 = (start_chain(&root), getnext, getnext, getdata, END);
	std::cout << val2 << std::endl;

	auto val3 = (start_chain(&root), getnext, getnext, getnext, getdata, END);
	std::cout << val3 << std::endl;
}
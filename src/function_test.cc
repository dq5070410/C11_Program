#include <iostream>
#include <functional>

using namespace std;

void call_when_even(int x,const std::function<void(int)>& f)
{
	if(!(x & 1))
	{
		f(x);
	}
}

void output(int x)
{
	cout << x << " ";
}

void output_add_2(int x)
{
	cout << x + 2 << " ";
}

void f(int a,int b,int c,int d,int e)
{
	cout << a << " " << b << " " << c << " " << d << " " << e <<endl;
}
int main()
{
	auto g = std::bind(f,10,20,std::placeholders::_2,5,std::placeholders::_1);
	g(3,7);
	return 0;
}

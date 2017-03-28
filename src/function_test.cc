#include <iostream>
#include <functional>

using namespace std;

class A
{
public:
	A(const std::function<void()>& f):callback_(f){}
	void notify(void){
		callback_();
	}
private:
	std::function<void()> callback_;
};

class Foo
{
public:
	void operator()(void){
		cout << __FUNCTION__ << endl;
	}
};

int main()
{
	Foo foo;
	A aa(foo);
	aa.notify();
	return 0;
}
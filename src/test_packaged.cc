#include <iostream>
#include <utility>
#include <future>
#include <thread>

int triple(int x) {return x*3;}

int main()
{
	std::packaged_task<int(int)> tsk(triple);

	std::future<int> fut = tsk.get_future();
	std::thread(std::move(tsk),100).detach();
	std::cout << "The triple of 100 is " << fut.get() << ".\n";

	tak.reset();
	fut = tsk.get_future();
	std::thread(std::move(tsk),200).detach();
	std::cout << "the triple of 200 is " << fut.get() << ".\n";
	return 0;
}
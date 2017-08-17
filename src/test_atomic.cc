#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

std::atomic_flag lock_stream = ATOMIC_FLAG_INIT
std::stringstream ss;

void append_number(int x)
{
	while(lock_stream.test_and_set())
	{

	}
	ss << " thread #" << x << "\n";
	lock_stream.clear();
}

int main()
{
	std::vector<std::thread> threads;
	for(int i = 0;i<=10;i++)
	{
		threads.push_back(std::thread(append_number,i));
	}
	for(auto& th:threads)
		th.join();

	std::cout << ss.str() << std::endl;
	return 0;
}
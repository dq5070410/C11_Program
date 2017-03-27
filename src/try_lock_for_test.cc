#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

std::timed_mutex mtx;

void fireworks()
{
	while(!mtx.try_lock_for(std::chrono::milliseconds(200)))
	{
		std::cout << "-";
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	std::cout << "*\n";
	mtx.unlock();
}

int main()
{
	std::thread threads[10];
	for(int i = 0;i<10;++i)
	{
		threads[i] = std::thread(fireworks);
	}
	for(auto& th:threads) th.join();
		
	return 0;
}
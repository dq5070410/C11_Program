#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

std::atomic<bool> ready(false);
std::atomic_flag winner = ATOMIC_FLAG_INIT;

void countlm(int id)
{
	while(!ready)
	{
		std::this_thread::yield();
	}
	for(int i = 0;i<100000;i++)
	{

	}
	if(!winner.test_and_set())
	{
		std::cout << "thread #" << id << "won!\n";
	}
};

int main()
{
	std::vector<std::thread> threads;
	std::cout << "spawning 10 threads that count to 1 millon...\n";
	for(int i = 0;i <= 10;++i)
		threads.push_back(std::thread(countlm,i));
	ready = true;
	for(auto & th:threads)
		th.join();
	return 0;
}
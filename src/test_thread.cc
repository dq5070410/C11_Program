#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <thread>
#include <iostream>

void thread_task(int n)
{
	std::this_thread::sleep_for(std::chrono::seconds(n));
	std::cout << "hello world"
	<< std::this_thread::get_id()
	<< "pause" << n << "seconds" << std::endl;
}

int main(int argc,const char *argv[])
{
	std::thread threads[5];
	std::cout << "Spawning 5 threads...\n";
	for(int i = 0;i < 5; i++)
	{
		threads[i] = std::thread(thread_task,i+1);
	}
	for(auto& t:threads)
		t.join();
	return 0;
}
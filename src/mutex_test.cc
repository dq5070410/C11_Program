#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex

volatile int count(0);

std::mutex mtx;

void attempt_10k_increase()
{
    for(int i = 0;i < 10000;i++)
    {
        if(mtx.try_lock())
        {
            count++;
            mtx.unlock();
        }
    }
}

int main()
{
    std::thread threads[10];
    for(int i = 0;i < 10;i++)
    {
        threads[i] = std::thread(attempt_10k_increase);
    }
    for(auto& th:threads)
        th.join();
    std::cout << count << "successful increase" << std::endl;
    return 0;
}
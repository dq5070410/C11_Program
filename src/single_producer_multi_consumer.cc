#include <unistd.h>
#include <cstdlib>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

static const int kItemRepositorySize = 4;
static const int kItemsToProduce = 10;

struct ItemRepository{
	int item_buffer[kItemRepositorySize];
	size_t read_position;
	size_t write_position;
	size_t item_counter;
	std::mutex mtx;
	std::mutex item_counter_mtx;
	std::condition_variable repo_not_full;
	std::condition_variable repo_not_empty;
} gItemRepository;

typedef struct ItemRepository ItemRepository;

void ProduceItem(ItemRepository *ir, int item)
{
	std::unique_lock<std::mutex> lock(ir->mtx);
	while((ir->write_position + 1) % kItemRepositorySize == ir->read_position)
	{
		std::cout << "ItemRepository is full,waiting for an empty slot...\n";
		(ir->repo_not_full).wait(lock);
	}

	(ir->item_buffer)[ir->write_position] = item;
	(ir->write_position)++;

	if(ir->write_position == kItemRepositorySize)
		ir->write_position = 0;
	(ir->repo_not_empty).notify_all();
	lock.unlock();
}

int ConsumeItem(ItemRepository *ir)
{
	std::unique_lock<std::mutex> lock(ir->mtx);
	while((ir->read_position) == ir->write_position)
	{
		std::cout << "ItemRepository is empty,waiting for a item...\n";
		(ir->repo_not_empty).wait(lock);
	}

	int data = ir->item_buffer[ir->read_position];
	(ir->read_position)++;

	if(ir->read_position == kItemRepositorySize)
		ir->read_position = 0;

	(ir->repo_not_full).notify_all();
	lock.unlock();
	return data;
}

void ProduceTask()
{
	for(int i = 0;i<=kItemsToProduce;++i)
	{
		std::cout << "Producer thread " << std::this_thread::get_id() << "producing the " << i << "^th item..." << std::endl;
		ProduceItem(&gItemRepository,i);
	}
	std::cout << "Producer thread " << std::this_thread::get_id() << " is exiting..." << std::endl;
}

void ConsumerTask()
{
	bool read_to_exit = false;
	while(1)
	{
		sleep(1);
		std::unique_lock<std::mutex> lock(gItemRepository.item_counter_mtx);
		if(gItemRepository.item_counter < kItemsToProduce)
		{
			int item = ConsumeItem(&gItemRepository);
			++(gItemRepository.item_counter);
			std::cout << "Consumer thread " << std::this_thread::get_id() << " is consuming the " << item << " ^th item" << std::endl;
		}
		else
			read_to_exit = true;
		lock.unlock();
		if(read_to_exit)
			break;
	}
	std::cout << "Consumer thread " << std::this_thread::get_id() << "is exiting..." << std::endl;
}

void InitItemRepository(ItemRepository *ir)
{
	ir->write_position = 0;
	ir->read_position = 0;
	ir->item_counter = 0;
}

int main()
{
	InitItemRepository(&gItemRepository);
	std::thread producer(ProduceTask);
	std::thread consumer1(ProduceTask);
	std::thread consumer2(ProduceTask);
	std::thread consumer3(ProduceTask);

	producer.join();
	consumer1.join();
	consumer2.join();
	consumer3.join();
	return 0;
}
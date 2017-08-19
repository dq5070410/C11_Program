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
		std::cout << "ItemRepository is full,waitinf for an empty slot...\n";
		(ir->repo_not_full).wait(lock);
	}

	(ir->item_buffer)[ir->write_position] = item;
	(ir->write_position)++;

	if(ir->write_position == kItemRepositorySize)
		ir->write_position = 0;
	(ir->repo_not_empty).notify_all();
	lock.unlock();
}
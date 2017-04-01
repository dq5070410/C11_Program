#include <unistd.h>
#include <cstdlib.h>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

static const int kItemRepositorySize = 10;
static const int kItemsToProduce = 1000;

struct ItemRepository{
	int item_buffer[kItemRepositorySize];
	size_t read_position;
	size_t write_position;
	std::mutex mtx;
	std::condition_variable repo_no_full;
	std::condition_variable repo_not_empty;
} gItemRepository;

typedef struct ItemRepository ItemRepository;

void ProduceItem(ItemRepository *ir,int item)
{
	std::unique_lock<std::mutex> lock(ir->mutex);
	while((ir->write_position + 1) % kItemRepositorySize == ir->read_position)
	{
		std::cout << "Producer is waiting for an empty slot...\n";
		(ir->repo_no_full).wait(lock);
	}
}
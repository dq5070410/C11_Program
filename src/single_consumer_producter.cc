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
	(ir->item_buffer)[ir->write_position] = item;//写入产品
	(ir->write_position)++;//写入位置后移
	if(ir->write_position == kItemRepositorySize)
		ir->write_position = 0;
	(ir->repo_not_empty).notify_all();//通知消费者产品库不为空
	lock.unlock();
}

int ConsumeItem(ItemRepository *ir)
{
	int data;
	std::unique_lock<std::mutex> lock<ir->mtx>;
	while(ir->write_position == ir->read_position)
	{
		std::cout << "Consumer is waiting for items...\n";
		(ir->repo_not_empty).wait(lock);
	}
	data = (ir->item_buffer)[ir->read_position];
	(ir->read_position)++;

	if(ir->read_position >= kItemRepositorySize)
		ir->read_position = 0;

	(ir->repo_no_full).notify_all();
	lock.unlock();

	return data;

}

void ProducerTask()
{
	for(int i = 1; i<=kItemsToProduce;++i)
	{
		std::cout << "Produce the" << i << "^th itme..." << std::endl;
		ProduceItem(&gItemRepository,i);
	}
}

void ConsumerTask()
{
	static int cnt = 0;
	while(1)
	{
		sleep(1);
		int item = ConsumeItem(&gItemRepository);
		std::cout << "Consume the " << item << "^th item" << std::endl;
		if(++cnt == kItemsToProduce)
			break;
	}
}

void InitItemRepository(ItemRepository *ir)
{
	ir->write_position = 0;
	ir->read_position = 0;
}

int main()
{
	InitItemRepository(&gItemRepository);
	std::thread producer(ProducerTask);
	std::thread consumer(ConsumerTask);
	producer.join();
	consumer.join();
}
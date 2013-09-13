#ifndef TWOLOCK_MUTEX_QUEUE_H
#define TWOLOCK_MUTEX_QUEUE_H

#include "config.h"
#include "queue.h"
#include "Chain.h"
#include <mutex>

#include <iostream>

class TLMutexQueue : public Queue {
	private:
	Chain *head;
	uint8_t paddingh[CACHE_ALIGN - sizeof(Chain*)];
   	Chain *tail;
	uint8_t paddingt[CACHE_ALIGN - sizeof(Chain*)];

	mutex *nqlock;
	mutex *dqlock;
	Chain *c;


	public:
	TLMutexQueue() INLINE_ATTR {
		c = new Chain(0);
		head = c;
		tail = c;
		nqlock = new mutex();
		dqlock = new mutex();
	}

	~TLMutexQueue(){
		delete c;
		delete nqlock;
		delete dqlock;
	}

	void enqueue(Chain *chain) INLINE_ATTR volatile
	{
		chain->next = 0;
		
		//lock or begin transaction;
		nqlock->lock();

			tail->next = chain;
			tail = chain;
		//unlock or end transaction;
		nqlock->unlock();
	}

	Chain* dequeue() INLINE_ATTR volatile
	{
		Chain *out;
		Chain *newhead;
		

		//lock or begin transaction;
		dqlock->lock();
			newhead	= head->next;
			if (newhead == 0){
				dqlock->unlock();	//unlock
				return 0;
			}
			head->data = newhead->data;
			out = head;
			head = newhead;
		//unlock or end transaction;
		dqlock->unlock();
		return out;
	}
};

#endif

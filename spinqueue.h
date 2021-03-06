#ifndef SPINLOCK_QUEUE_H
#define SPINLOCK_QUEUE_H

#include "config.h"
#include "queue.h"
#include "Chain.h"
#include "spinlock.h"

#include <iostream>

class SpinQueue : public Queue {
	private:
    Chain *head;
	uint8_t paddingh[CACHE_ALIGN - sizeof(Chain*)];
    Chain * volatile *tail;
	uint8_t paddingt[CACHE_ALIGN - sizeof(Chain*)];

	Spinlock *qlock;

	public:
	SpinQueue() INLINE_ATTR : head(0), tail(&head) {
		qlock = new Spinlock();
	}

	~SpinQueue() {
		delete qlock;
	}

	void enqueue(Chain *chain) INLINE_ATTR volatile
	{
		chain->next = 0;
		//lock or begin transaction;
		qlock->lock();
			*tail = chain;
			tail = &chain->next;
		//unlock or end transaction;
		qlock->unlock();
	}

	Chain* dequeue() INLINE_ATTR volatile
	{
		Chain *out;

		//lock or begin transaction;
		qlock->lock();
			if((out = head) && !(head = out->next))
			{
				tail = &head;
			}
		//unlock or end transaction;
		qlock->unlock();

		return out;
	}
};

#endif

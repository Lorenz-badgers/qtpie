#ifndef QUEUE_H
#define QUEUE_H

#include "Chain.h"

using namespace std;
class Queue{
	public:
	virtual void enqueue(Chain* chain) volatile{
		cout << "volatile fehlt"<< endl;
	}
	virtual Chain* dequeue() volatile{
		cout << "volatile fehlt"<< endl;
	}
};

#endif

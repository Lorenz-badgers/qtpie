#ifndef __MPSCQUEUE_H__
#define __MPSCQUEUE_H__

#include "Chain.h"


#include "queue.h"


#ifndef WOSCH_CAS
	#define WOSCH_CAS(p, o, n) \
		__sync_bool_compare_and_swap((uintptr_t*) (p), (uintptr_t) (o), (uintptr_t) (n))
#endif


class MPSCQueue : public Queue {
	Chain** tail;
	Chain* volatile next;
public:
	//MPSCQueue() : Chain(0), tail((Chain**volatile)&next) { }
	MPSCQueue() : next(0), tail((Chain**volatile)&next) { }
	void enqueue(Chain* item) volatile;
	Chain* dequeue() volatile;
	Chain* peek() const { return (Chain*) next; }
};

inline void MPSCQueue::enqueue(Chain* item) volatile{
	Chain** volatile last;
	item->next = 0;		/* make item last chain element */
	do {
		last = tail;
	} while (!WOSCH_CAS(&tail, last, &(item->next)));
	*last = item;		//CAS successful, so I can now link safely
}

inline Chain* MPSCQueue::dequeue() volatile{
	Chain* volatile item;
	if ((item = next) && !(next = item->next)) {
		if (!WOSCH_CAS(&tail, &(item->next), &next)) {
			while(item->next == 0) { }
			next = item->next;
		}
	}
	return (Chain*)item;
}

#endif

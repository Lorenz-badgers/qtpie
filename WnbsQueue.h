#ifndef __LIB_ADT_WNBSQUEUE_H__
#define __LIB_ADT_WNBSQUEUE_H__

#include "Chain.h"
#include "queue.h"

#ifndef WOSCH_CAS
	#define WOSCH_CAS(p, o, n) \
		__sync_bool_compare_and_swap((uintptr_t*) (p), (uintptr_t) (o), (uintptr_t) (n))

#endif


typedef Chain Chain_p;

class WnbsQueue : public Queue {
	volatile Chain head;
	volatile Chain* tail;
	static const uintptr_t ABA_MASK = 0x1;
public:
	WnbsQueue() { head.next = 0; tail = &head; }
	void enqueue(Chain_p *item) volatile;
	Chain_p* dequeue() volatile;
	static Chain *abaIndex (Chain_p* item) {
		return (Chain *)((uintptr_t)item & ~ABA_MASK);
	}

private: 
	static Chain_p* aba_wheel (Chain_p* item) {
		return ((uintptr_t)item ^ ABA_MASK == 1 ? (Chain_p *)1 : (Chain_p *)0);
	}

};


void WnbsQueue::enqueue(Chain_p *item) volatile{
	volatile Chain* last;
	volatile Chain* self;

	// assert(this && aba_index(item));

	item->next = aba_wheel(item);	/* item becomes new tail resp. next last */

	do {
		self = (last = this->tail)->next;		/* draw copies as needed */
	} while (!WOSCH_CAS(&this->tail, last, item));

	if (!WOSCH_CAS(&last->next, self, item)) {		/* last removed by fetch */
		this->head.next = item;			/* item becomes new head */
	}
}

Chain_p* WnbsQueue::dequeue() volatile{
	Chain_p* node;
	Chain_p* next;

	do {
		node = this->head.next;
		if (abaIndex((node)) == 0) return 0;
		next = abaIndex(node)->next;
	}
	while (!WOSCH_CAS(&this->head.next, node, (next == node ? 0 : next)));

	if (next == node) {	/* last element just removed, be careful */
		if (!WOSCH_CAS(&node->next, next, 0)) this->head.next = abaIndex(node)->next;
		else WOSCH_CAS(&this->tail, &node->next, &this->head);
	}

	return node;
}

#endif

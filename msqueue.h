#pragma once

#include "config.h"
#include "constants.h"
#include "cpointer.h"
#include "queue.h"
#include "Chain.h"

#define T Chain

class MSQueue : public Queue
{
	public:
	volatile CPointer<Chain> head;
	volatile CPointer<Chain> tail;
	Chain dummy;

	MSQueue() INLINE_ATTR {
		head.setPtr(&dummy);
		tail.setPtr(&dummy);
	}

	void enqueue(Chain *chain) volatile INLINE_ATTR
	{
		chain->cnext.setCounter(chain->cnext.getCounter() + 1);//incrementing the counter may not be important... do it anyways
		chain->cnext.setPtr(0);//set next to 0, indicating the last element of the queue

		for(;;)
		{
			//get local copies
			CPointer<Chain> ctail = tail;
			Chain* ptr = ctail.getPtr();
		//	if (ptr == 0 )
		//		cout << "fehler in nq" <<endl;
			CPointer<Chain> next = ptr->cnext;

			//this is important, check if copies are consistent
			if(unlikely(ctail != tail))
				continue;

			if(next.getPtr() == 0)//enqueueing at end of queue
			{
				CPointer<Chain> cnext(chain, next.getCounter() + 1);
				if(likely(CPointer<Chain>::CAS(&(ctail.getPtr()->cnext), next, cnext)))
				{
					//this is not needed in general, but in this system, leave it here
					CPointer<Chain> tnext(chain, ctail.getCounter() + 1);
					CPointer<Chain>::CAS(&(tail), ctail, tnext);
					//
					break;//successfully enqueued, done
				}

				continue;
			}

			//else tail is falling behind
			//advance it to next
			CPointer<Chain> tnext(next.getPtr(), ctail.getCounter() + 1);
			CPointer<Chain>::CAS(&(tail), ctail, tnext);
		}
	}

	Chain* dequeue() volatile INLINE_ATTR
	{
		for(;;)
		{
			//get local copies
			CPointer<Chain> chead = head;
			CPointer<Chain> ctail = tail;

			Chain* ptr = chead.getPtr();
		//	if (ptr == 0 )
		//		cout << "fehler in dq" <<endl;
			CPointer<Chain> next = ptr->cnext;

			//this is important, check if copies are consistent
			if(unlikely(chead != head))
				continue;

			if(chead.getPtr() == ctail.getPtr())//only one element in the queue
			{
				if(next.getPtr() == 0)//only the dummy node, queue is empty
					return 0;

				//tail is falling behind, advance it, try again
				CPointer<Chain> tnext(next.getPtr(), ctail.getCounter() + 1);
				CPointer<Chain>::CAS(&(tail), ctail, tnext);
				continue;
			}
			
			void *out = next.getPtr()->data;
			//else there are elements in the queue try to advance head pointer
			CPointer<Chain> hnext(next.getPtr(), chead.getCounter() + 1);
			if(likely(CPointer<Chain>::CAS(&(head), chead, hnext)))
			{
				//here we successfully dequeued an element
				//the pointer is volatile because &(dummy)
				//is volatile too and they get compared later
				Chain * volatile outchain = chead.getPtr();

				outchain->data=out;
				return outchain;
				//incrementing the counter may not be important... do it anyways
			}
		}
	}
} __attribute__ ((aligned (CACHE_ALIGN)));


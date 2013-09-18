
#include <iostream>
#include <fstream>
#include <climits>

#include "Chain.h"
#include "tests.h"
#include "queues.h"



//#define HASWELL
#define CORES 8

#ifdef HASWELL
#define HASWELLQUEUES(code) {	\
	code(TransQueue);	\
	code(TLTransQueue);	\
}	
#else
#define HASWELLQUEUES(code)
#endif				

#define avg(retmin, retavg, n, fun, argsa...) {	\
	int i=n;			\
	long retavg2;			\
	retavg =0;			\
	retmin = LONG_MAX;		\
	while(i-->0){			\
		fun(retavg2, argsa);	\
		retavg += retavg2;	\
		if(retavg2 < retmin)	\
		retmin = retavg2;	\
	}				\
	retavg /=n;			\
}


#define singletest(retsingle, QType, TType, args...) {	\
	QType *queue = new QType;			\
	TType t(queue, args);				\
	retsingle = t.starttest();			\
	t.cleanup();					\
	delete queue;					\
}

#define iterqueues(code) {	\
	code(SpinQueue);	\
	code(TLSpinQueue);	\
	code(MutexQueue);	\
	code(TLMutexQueue);	\
	code(MSQueue);		\
	HASWELLQUEUES(code);	\
}

#define iterqueuesMPSC(code) {	\
	iterqueues(code)	\
	code(MPSCQueue);	\
}

#define testandprintround(TQueue) {				\
		long reta=0;					\
		long retm=0;					\
		avg(retm, reta, iterations, singletest, TQueue, Test_round, (threads*percent-1)/100 + 1, threads, ops);\
		minstream << retm << "\t";				\
		avgstream << reta << "\t";				\
}

#define testandprintmpsc(TQueue) {				\
		long retavg=0;					\
		long retmin=0;					\
		avg(retmin, retavg, iterations, singletest, TQueue, Test_mpmc, threads, 1, ops);\
		minstream << retmin << "\t";				\
		avgstream << retavg << "\t";				\
}

#define testandprintmpmc(TQueue) {				\
		long retavg=0;					\
		long retmin=0;					\
		avg(retmin, retavg, iterations, singletest, TQueue, Test_mpmc, threads, threads, ops);\
		minstream << retmin << "\t";				\
		avgstream << retavg << "\t";				\
}

#define printname(Object) {		\
	minstream << #Object << "\t"; 	\
	avgstream << #Object << "\t"; 	\
}

using namespace std;


void roundall(long percent, long ops, int iterations, ofstream &minstream, ofstream &avgstream){
	minstream << "#min-Testround,\tInhalt = Threads * " << percent << "%\t Operationen: " << ops << endl;
	minstream << "#t\t";
	avgstream << "#avg-Testround,\tInhalt = Threads * " << percent << "%\t Operationen: " << ops << endl;
	avgstream << "#t\t";
	iterqueues(printname);
	minstream << endl;
	avgstream << endl;
	for (int threads = 1; threads <= 2*CORES; threads++){	
		minstream << threads << "\t";			
		avgstream << threads << "\t";			
		iterqueues(testandprintround);
		minstream << endl;				
		avgstream << endl;				
	}
}

void mpscall(long ops, int iterations, ofstream &minstream, ofstream &avgstream){
	minstream << "#min-Testmpsc,\t Operationen: " << ops << endl;
	minstream << "#t\t";
	avgstream << "#avg-Testmpsc,\t Operationen: " << ops << endl;
	avgstream << "#t\t";
	iterqueuesMPSC(printname);
	minstream << endl;
	avgstream << endl;
	for (int threads = 1; threads <= 2*CORES; threads++){	
		minstream << threads << "\t";			
		avgstream << threads << "\t";			
		iterqueuesMPSC(testandprintmpsc);
		minstream << endl;				
		avgstream << endl;				
	}
}

void mpmcall(long ops, int iterations, ofstream &minstream, ofstream &avgstream){
	minstream << "#min-Testmpmc,\t Operationen: " << ops << endl;
	minstream << "#t\t";
	avgstream << "#avg-Testmpmc,\t Operationen: " << ops << endl;
	avgstream << "#t\t";
	iterqueuesMPSC(printname);
	minstream << endl;
	avgstream << endl;
	for (int threads = 1; threads <= 2*CORES; threads++){	
		minstream << threads << "\t";			
		avgstream << threads << "\t";			
		iterqueues(testandprintmpmc);
		minstream << endl;				
		avgstream << endl;				
	}
}

int main(){
	/*usecase 1:
	avg(retval, n, fun, args...) 
	n = iterations
	fun = singletest(retval, QType, TType, args...) 	
	TType(args) = Test_round(inhalt, threads, ops)
	TType(args) = Test_mpmc(producers, consumers, ops)
	*/

/* example:
	long ret=0;
	avg(ret, ITERATIONS, singletest, SpinQueue, Test_round, 20, 8, 1<<20);
	cout << ret << "\t";
	cout endl;
*/

	/*usecase 2:
	roundall(percent, ops, iterations, min-ostream, avg-ostream);	
	mpscall(ops, iterations,  min-ostream, avg-ostream);	
	mpmcall(ops, iterations,  min-ostream, avg-ostream);	
	*/


	ofstream minroundstream1;
	ofstream minroundstream2;
	ofstream minroundstream3;
	ofstream minmpscstream;
	ofstream minmpmcstream;

	ofstream avgroundstream1;
	ofstream avgroundstream2;
	ofstream avgroundstream3;
	ofstream avgmpscstream;
	ofstream avgmpmcstream;

#define ITR 50

	minroundstream1.open("./data/roundallmin1");
	avgroundstream1.open("./data/roundallavg1");
	roundall(50, 1<<20, ITR, minroundstream1, avgroundstream1);
	minroundstream1.close();
	avgroundstream1.close();

	minroundstream2.open("./data/roundallmin2");
	avgroundstream2.open("./data/roundallavg2");
	roundall(100, 1<<20, ITR, minroundstream2, avgroundstream2);
	minroundstream2.close();
	avgroundstream2.close();

	minroundstream3.open("./data/roundallmin3");
	avgroundstream3.open("./data/roundallavg3");
	roundall(200, 1<<20, ITR, minroundstream3, avgroundstream3);
	minroundstream3.close();
	avgroundstream3.close();

	minmpscstream.open("./data/mpscstreammin");
	avgmpscstream.open("./data/mpscstreamavg");
	mpscall(1<<20, ITR, minmpscstream, avgmpscstream);
	minmpscstream.close();
	avgmpscstream.close();

	minmpmcstream.open("./data/mpmcstreammin");
	avgmpmcstream.open("./data/mpmcstreamavg");
	mpmcall(1<<20, ITR, minmpmcstream, avgmpmcstream);
	minmpmcstream.close();
	avgmpmcstream.close();
}

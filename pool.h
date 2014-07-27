#ifndef ____POOL_H____
#define ____POOL_H____

#include <iostream>
#include <string>
#include <pthread.h>

using namespace std;

/* 
 * A pool is a circular queue with messages.
 */


template<class T, int SZ=1200>
class cPool 
{
    pthread_mutex_t lock;
	T pool[SZ];
    short index;
    short size;
public:
	cPool():size(SZ), index(0) { lock=PTHREAD_MUTEX_INITIALIZER; }

	~cPool() { pthread_mutex_destroy(&lock); }

	T * Malloc(int ref_count=1)
	{
		T *p = 0 ;
		pthread_mutex_lock(&lock); 
		if(!size) 
		{
			pthread_mutex_unlock(&lock); 
		}
		else 
		{
			p = &pool[index];
			if(ref_count > 1) { 
				pool[index].internal_use_only_isShared=true;
				pool[index].internal_use_only_SharedCnt = ref_count;
			}
			size--;
			index++;
			if(index == SZ) index=0;
			pthread_mutex_unlock(&lock);
		}
		return p;
	}
	void Free(T* p)
	{
		pthread_mutex_lock(&lock);
		if(p->internal_use_only_isShared && p->internal_use_only_SharedCnt!=1)
		{
			p->internal_use_only_SharedCnt--;
		}
		else 
		{
			p->internal_use_only_isShared=false; 
			p->internal_use_only_SharedCnt=0;
			size++;
		}
		pthread_mutex_unlock(&lock);
	}
};

#endif //_____POOL_H____
/* g++ -std=c++11 -lpthread <>.cpp */

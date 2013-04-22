#include <iostream>
#include <stdlib.h>
#include <proc/sysinfo.h>
#include <string.h>
#include <deque>
#include <queue>
#include <unistd.h>

/**
*	Author: Robert Metzger (metzgerr@web.de)
*
* The purpose of this tool is to constantly allocate n MB of main memory
* on a linux machine. 
* I've used this tool to do I/O hard disk benchmarks and we wanted to have
* a fixed amount of page cache pages around.
*
*/

using namespace std;

struct mem {
	int mb_cached;
	int mb_buffers;
	int mb_free;
};

mem m_info;

void update_m_info() {
	meminfo();
	m_info.mb_cached = kb_main_cached/1024;
	m_info.mb_buffers = kb_main_buffers/1024;
	m_info.mb_free = kb_main_free/1024;
	//cout << "Cached: " << m_info.mb_cached << " Buffers: " << m_info.mb_buffers
	//	<< " Free: " << m_info.mb_free << "\r";
}


int main(int argc, char* argv[])
{
	// info from /proc/meminfo
	m_info.mb_cached = 0;
	m_info.mb_buffers = 0;
	m_info.mb_free = 0;
	// the amount of mem the user wants for the cache.
	int desiredCacheSize = -1;

	deque<void*>*  allocated_memory = new deque<void*>;

	if(argc <= 1) {
		cout << "Usage: ballooner <desiredCacheSizeMegaBytes>" << endl;
	} else {
		desiredCacheSize = strtol(argv[1], NULL, 10);
		if(desiredCacheSize <= 0) {
			cout << "Values equal or lower to zero are not supported";
			return 1;
		}

		update_m_info();

		int alloc_count = 0;
		int alloc_sum = 0;
		int avg = -1;
		while(true) {
			if(m_info.mb_free+m_info.mb_cached > desiredCacheSize) {

				// we need to allocate memory
				int alloc_mb = (m_info.mb_cached+m_info.mb_free)/50; // edit this param for finer alloc granularity
				if(avg != -1 && alloc_mb < avg) {
					usleep(5000); // sleep for 0.005 seconds
					goto endofLoop;
				}
				long amount = (long)alloc_mb*1024*1024;
				cout << "Allocating " << alloc_mb << " MB " << endl;
				void * buffer =  malloc ( amount );
				memset(buffer, 1, amount);
				allocated_memory->push_front(buffer);

				if(avg == -1) {
					alloc_count++;
					alloc_sum += alloc_mb;
					cout << "Avg: " << alloc_sum/alloc_count << endl;
				}
				if(alloc_count == 200) { // set this parameter higher, to have a more accurate "sleep phase"
					avg = alloc_sum/alloc_count;
				}
			} else {
				// free up again.
				if(allocated_memory->size() == 0) {
					cout << "No memory allocated!" << endl;
					goto endofLoop;
				} 
				void* somemem = allocated_memory->back();
				allocated_memory->pop_back();
				if(somemem != NULL) {
					cout << "Freeing " << somemem << endl;
					free(somemem);
				} else {
					cout << "Can not free more memory! Exiting...";
					return 0;
				}
			}
			endofLoop:
			// sleep(1); // don't stress the kernel too much

			update_m_info();
		}
	}
}
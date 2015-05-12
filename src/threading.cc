// Implementation of multi-threaded SME-execution.  The goal of this
// implementation is to be able to perform multi-threaded execution of
// SME-networks

// The threading-module will get a list of all processes and schedule
// them across CPU-threads.  We can utilize various optimization
// strategies for making sure that we minimize the time each CPU-core
// stays idle.
//
// One major challenge is how to deal with this on an highly-loaded
// system. We need to use some kind of statistical average so that
// one, outlying, poorly performing iteration want trigger an
// unnecessary process re-balancing.

#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <chrono>

//#include "queue.h"
#include "bqueue.h"
#include "sme.h"
#include "threading.h"
#include "runner.h"

ThreadedRun::ThreadedRun(int steps, int threads)
  :Runner(steps) {
    if (threads < 1) {
      char* sme_threads_env = std::getenv("SME_THREADS");
      if (sme_threads_env != nullptr) {
	this->threads = std::stoi(sme_threads_env, 0, 10);
	std::cout << "Running " << this->threads << " thread(s) as specified by environment variable\n";
      } else {
	this->threads = std::thread::hardware_concurrency();
	if (this->threads == 0) {
	  std::cout << "Unable to automatically determine level of hardware concurrency, setting to 1 (single threaded mode" << std::endl;
	  this->threads = 1;
	} else {
	  std::cout << "Detected " << this->threads << " threads on this system\n";
	}
      }
    }
}

    //ThreadedRun::ThreadedRun(int steps) {
    //  ThreadedRun(steps, 0);
    //}

void ThreadedRun::instance(BQueue* q, int id) {
  SyncProcess* p = q->next(id);
  while (p != nullptr) {
    p->step();
    p = q->next(id);
    //std::cout << "step\n";
  }
}

/*void ThreadedRun::blocking() {
  }*/

void ThreadedRun::start() {
  // It makes no sense to have more threads than network processes
  if (procs.size() < threads) {
    threads = procs.size();
    std::cout << "Less network processes than threads. Limiting number of threads to "\
	      << threads << std::endl;
  }
  auto q = BQueue(threads, steps);
  q.populate(procs, busses);

  auto start = std::chrono::high_resolution_clock::now();
  std::thread** t = new std::thread* [threads];
  //std::cout << "----------------------+++++" << threads;
  for(unsigned i = 0; i < threads; ++i) {
    t[i] = new std::thread(instance, &q, i);
  }

  for(unsigned int i = 0; i < threads; ++i) {
    t[i]->join();
    delete t[i];
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = end-start;
  std::cout << "Executed network in " << diff.count() << "s\n";
  delete[] t;
}
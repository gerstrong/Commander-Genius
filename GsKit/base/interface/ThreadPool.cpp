/*
 *  ThreadPool.cpp
 *  OpenLieroX
 *
 *  Created by Albert Zeyer on 08.02.09.
 *  code under LGPL
 *
 */

#include <SDL_thread.h>

#include "base/GsLogging.h"

#include "ThreadPool.h"
#include "Debug.h"
#include "ReadWriteLock.h" // for ScopedLock


ThreadPool::ThreadPool(const unsigned int size) {
    nextAction = nullptr; nextIsHeadless = false; nextData = NULL;
	quitting = false;
	mutex = SDL_CreateMutex();
	awakeThread = SDL_CreateCond();
	threadStartedWork = SDL_CreateCond();
	threadStatusChanged = SDL_CreateCond();
	startMutex = SDL_CreateMutex();

	notes << "ThreadPool: creating " << size << " threads ..." << endl;
	while(availableThreads.size() < size)
		prepareNewThread();
}


ThreadPool::~ThreadPool() {
	waitAll();

	// this is the hint for all available threads to break
	SDL_mutexP(mutex); // lock to be sure that every thread is outside that region, we could get crashes otherwise
    nextAction = nullptr;
	quitting = true;
	SDL_CondBroadcast(awakeThread);
    for(auto &threadItem : availableThreads)
    {
		SDL_mutexV(mutex);
        SDL_WaitThread(threadItem->thread, NULL);
		SDL_mutexP(mutex);
        SDL_DestroyCond(threadItem->finishedSignal);
        SDL_DestroyCond(threadItem->readyForNewWork);
        delete threadItem;
	}
	availableThreads.clear();
	SDL_mutexV(mutex);

	SDL_DestroyMutex(startMutex);
	SDL_DestroyCond(threadStartedWork);
	SDL_DestroyCond(threadStatusChanged);
	SDL_DestroyCond(awakeThread);
	SDL_DestroyMutex(mutex);
}

void ThreadPool::prepareNewThread() {
	ThreadPoolItem* t = new ThreadPoolItem();
	t->pool = this;
	t->finishedSignal = SDL_CreateCond();
	t->readyForNewWork = SDL_CreateCond();
	t->finished = false;
	t->working = false;

    availableThreads.insert(t);

#if SDL_VERSION_ATLEAST(2, 0, 0)
    const unsigned int numThreads = availableThreads.size();
    const std::string threadName = "threadItem" + to_string(numThreads);

    SDL_Thread *sdlThread = SDL_CreateThread(threadWrapper, threadName.c_str(), t);

    if(!sdlThread)
    {
        gLogging.ftextOut("Thread creation failed: %s\n", SDL_GetError());
        return;
    }

    t->thread = sdlThread;
#else
    t->thread = SDL_CreateThread(threadWrapper, t);
#endif
}

int ThreadPool::threadWrapper(void* param)
{
	ThreadPoolItem* data = (ThreadPoolItem*)param;

	SDL_mutexP(data->pool->mutex);
    while(true)
    {
        while(data->pool->nextAction == nullptr && !data->pool->quitting)
			SDL_CondWait(data->pool->awakeThread, data->pool->mutex);
		if(data->pool->quitting) break;
		data->pool->usedThreads.insert(data);
		data->pool->availableThreads.erase(data);

        Action* act = data->pool->nextAction; data->pool->nextAction = nullptr;
		data->headless = data->pool->nextIsHeadless;
		data->name = data->pool->nextName;
		data->finished = false;
		data->working = true;
		data->pool->nextData = data;
		SDL_mutexV(data->pool->mutex);

		SDL_CondSignal(data->pool->threadStartedWork);
        gLogging << "Running Thread: " << data->name << CLogFile::endl;
		data->ret = act->handle();
		delete act;
        gLogging << data->name + " [finished]" << CLogFile::endl ;
		SDL_mutexP(data->pool->mutex);
		data->finished = true;
		SDL_CondSignal(data->pool->threadStatusChanged);

		if(!data->headless) { // headless means that we just can clean it up right now without waiting
			SDL_CondSignal(data->finishedSignal);
			while(data->working) SDL_CondWait(data->readyForNewWork, data->pool->mutex);
		} else
			data->working = false;
		data->pool->usedThreads.erase(data);
		data->pool->availableThreads.insert(data);
		SDL_CondSignal(data->pool->threadStatusChanged);
		//setCurThreadName("");
	}

	SDL_mutexV(data->pool->mutex);

	return 0;
}

ThreadPoolItem* ThreadPool::start(Action* act, const std::string& name, bool headless)
{
	SDL_mutexP(startMutex); // If start() method will be called from different threads without mutex, hard-to-find crashes will occur
	SDL_mutexP(mutex);
	if(availableThreads.size() == 0) {
		warnings << "no available thread in ThreadPool for " << name << ", creating new one..." << endl;
		prepareNewThread();
	}
	assert(nextAction == NULL);
	assert(nextData == NULL);
	nextAction = act;
	nextIsHeadless = headless;
	nextName = name;

	SDL_CondSignal(awakeThread);
	while(nextData == NULL) SDL_CondWait(threadStartedWork, mutex);
	ThreadPoolItem* data = nextData; nextData = NULL;
	SDL_mutexV(mutex);

	SDL_mutexV(startMutex);
	return data;
}

ThreadPoolItem* ThreadPool::start(ThreadFunc fct, void* param, const std::string& name) {
	struct StaticAction : Action {
		ThreadFunc fct; void* param;
		int handle() { return (*fct) (param); }
	};
	StaticAction* act = new StaticAction();
	act->fct = fct;
	act->param = param;
	ThreadPoolItem* item = start(act, name);
	if(item) return item;
	delete act;
	return NULL;
}

bool ThreadPool::wait(ThreadPoolItem* thread, int* status) {
	if(!thread) return false;
	SDL_mutexP(mutex);
	if(!thread->working) {
		warnings << "given thread " << thread->name << " is not working anymore" << endl;
		SDL_mutexV(mutex);
		return false;
	}
	while(!thread->finished) SDL_CondWait(thread->finishedSignal, mutex);
	if(status) *status = thread->ret;
	thread->working = false;
	SDL_mutexV(mutex);

	SDL_CondSignal(thread->readyForNewWork);
	return true;
}


bool ThreadPool::finalizeIfReady(ThreadPoolItem* thread, int* status) {
	if(!thread) return false;
	SDL_mutexP(mutex);

	if(!thread->working) {
		warnings << "given thread " << thread->name << " is not working anymore" << endl;
		SDL_mutexV(mutex);
		return false;
	}

	if(thread->finished) {
		if(status) *status = thread->ret;
		thread->working = false;
		SDL_mutexV(mutex);

		SDL_CondSignal(thread->readyForNewWork);
		return true;
	}

	SDL_mutexV(mutex);
	return false;
}


bool ThreadPool::waitAll() {
	SDL_mutexP(mutex);
	while(usedThreads.size() > 0) {
        warnings << "ThreadPool: waiting for " << usedThreads.size() << " thread(s) to finish:" << endl;
		for(std::set<ThreadPoolItem*>::iterator i = usedThreads.begin(); i != usedThreads.end(); ++i) {
			if((*i)->working && (*i)->finished) {
				warnings << "  thread " << (*i)->name << " is ready but was not cleaned up" << endl;
				(*i)->working = false;
				SDL_CondSignal((*i)->readyForNewWork);
			}
			else if((*i)->working && !(*i)->finished) {
				warnings << "  thread " << (*i)->name << " is still working" << endl;
			}
			else if(!(*i)->working && !(*i)->headless && (*i)->finished) {
				warnings << "  thread " << (*i)->name << " is cleaning itself up right now" << endl;
			}
			else {
				warnings << "  thread " << (*i)->name << " is in an invalid state" << endl;
			}
		}
		SDL_CondWait(threadStatusChanged, mutex);
	}
	SDL_mutexV(mutex);

	return true;
}

/*void ThreadPool::dumpState(CmdLineIntf& cli) const {
	ScopedLock lock(mutex);
	for(std::set<ThreadPoolItem*>::const_iterator i = usedThreads.begin(); i != usedThreads.end(); ++i) {
		if((*i)->working && (*i)->finished)
			cli.writeMsg("thread '" + (*i)->name + "': ready but was not cleaned up");
		else if((*i)->working && !(*i)->finished)
			cli.writeMsg("thread '" + (*i)->name + "': working");
		else if(!(*i)->working && !(*i)->headless && (*i)->finished)
			cli.writeMsg("thread '" + (*i)->name + "': cleanup");
		else
			cli.writeMsg("thread '" + (*i)->name + "': invalid");
	}
}*/


std::unique_ptr<ThreadPool> gThreadPool;

void InitThreadPool(const unsigned int size)
{
    if(!gThreadPool)
    {
        gThreadPool.reset(new ThreadPool(size));
    }
	else
    {
		errors << "ThreadPool inited twice" << endl;
    }
}

void UnInitThreadPool()
{
    if(gThreadPool) {
        gThreadPool = nullptr;
    } else {
		errors << "ThreadPool already uninited" << endl;
    }
}



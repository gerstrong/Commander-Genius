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


ThreadPool::ThreadPool() {
    nextAction = nullptr;
    nextIsHeadless = false;
    nextData = nullptr;
	quitting = false;
	mutex = SDL_CreateMutex();
	awakeThread = SDL_CreateCond();
	threadStartedWork = SDL_CreateCond();
	threadStatusChanged = SDL_CreateCond();
	startMutex = SDL_CreateMutex();
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
	}

	availableThreads.clear();
	SDL_mutexV(mutex);

	SDL_DestroyMutex(startMutex);
	SDL_DestroyCond(threadStartedWork);
	SDL_DestroyCond(threadStatusChanged);
	SDL_DestroyCond(awakeThread);
	SDL_DestroyMutex(mutex);
}

void ThreadPool::prepareNewThreads(const unsigned int size)
{
    notes << "ThreadPool: creating " << size << " threads ..." << endl;
    while(availableThreads.size() < size)
        prepareNewThread();
}

void ThreadPool::prepareNewThread() {

    auto t = std::make_shared<ThreadPoolItem>();
	t->finishedSignal = SDL_CreateCond();
	t->readyForNewWork = SDL_CreateCond();
	t->finished = false;
	t->working = false;    

    availableThreads.insert(t);

#if SDL_VERSION_ATLEAST(2, 0, 0)
    const unsigned int numThreads = availableThreads.size();
    const std::string threadName = "threadItem" + to_string(numThreads);

    // Note: Be careful, the address of the shared ptr is passed. Needed later when we cast back through
    // The thread wrapper
    SDL_Thread *sdlThread = SDL_CreateThread(threadWrapper, threadName.c_str(), t.get());

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
    ThreadPoolItem *raw = reinterpret_cast<ThreadPoolItem*>(param);

    auto pool = gThreadPool;
    assert(pool);

    SDL_mutexP(pool->mutex);

    // find the right shared pointer to it.
    std::shared_ptr<ThreadPoolItem> item;
    for(auto &t : pool->availableThreads)
    {
        if(t.get() == raw)
        {
            item = t;
            break;
        }
    }

    assert(item);

    while(true)
    {
        while(pool->nextAction == nullptr && !pool->quitting)
            SDL_CondWait(pool->awakeThread, pool->mutex);
        if(pool->quitting) break;
        pool->usedThreads.insert(item);
        pool->availableThreads.erase(item);

        Action* act = pool->nextAction; pool->nextAction = nullptr;
        item->headless = pool->nextIsHeadless;
        item->name = pool->nextName;
        item->finished = false;
        item->working = true;
        pool->nextData = item;
        SDL_mutexV(pool->mutex);

        SDL_CondSignal(pool->threadStartedWork);
        gLogging << "Running Thread: " << item->name << CLogFile::endl;
        item->ret = act->handle();
		delete act;
        gLogging << item->name + " [finished]" << CLogFile::endl ;
        SDL_mutexP(pool->mutex);
        item->finished = true;
        SDL_CondSignal(pool->threadStatusChanged);

        if(!item->headless) { // headless means that we just can clean it up right now without waiting
            SDL_CondSignal(item->finishedSignal);
            while(item->working) SDL_CondWait(item->readyForNewWork, pool->mutex);
        } else {
            item->working = false;
        }

        pool->usedThreads.erase(item);
        pool->availableThreads.insert(item);
        SDL_CondSignal(pool->threadStatusChanged);
		//setCurThreadName("");
	}

    SDL_mutexV(pool->mutex);

	return 0;
}

std::shared_ptr<ThreadPoolItem>
ThreadPool::start(Action* act, const std::string& name, const bool headless)
{
	SDL_mutexP(startMutex); // If start() method will be called from different threads without mutex, hard-to-find crashes will occur
	SDL_mutexP(mutex);
	if(availableThreads.size() == 0) {
		warnings << "no available thread in ThreadPool for " << name << ", creating new one..." << endl;
		prepareNewThread();
	}
    assert(nextAction == nullptr);
    assert(nextData == nullptr);
	nextAction = act;
	nextIsHeadless = headless;
	nextName = name;

	SDL_CondSignal(awakeThread);
    while(nextData == nullptr) SDL_CondWait(threadStartedWork, mutex);
    std::shared_ptr<ThreadPoolItem> data = nextData;
    nextData = nullptr;
	SDL_mutexV(mutex);

	SDL_mutexV(startMutex);
	return data;
}

std::shared_ptr<ThreadPoolItem>
ThreadPool::start(ThreadFunc fct, void* param, const std::string& name)
{
	struct StaticAction : Action {
		ThreadFunc fct; void* param;
		int handle() { return (*fct) (param); }
	};
	StaticAction* act = new StaticAction();
	act->fct = fct;
	act->param = param;
    auto item = start(act, name);
	if(item) return item;
	delete act;
    return nullptr;
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


bool ThreadPool::finalizeIfReady(std::shared_ptr<ThreadPoolItem> &thread, int* status) {
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
        for (auto &t : usedThreads)
        {
            if(t->working && t->finished) {
                warnings << "  thread " << t->name << " is ready but was not cleaned up" << endl;
                t->working = false;
                SDL_CondSignal(t->readyForNewWork);
            }
            else if(t->working && !t->finished) {
                warnings << "  thread " << t->name << " is still working" << endl;
            }
            else if(!t->working && !t->headless && t->finished) {
                warnings << "  thread " << t->name << " is cleaning itself up right now" << endl;
            }
            else {
                warnings << "  thread " << t->name << " is in an invalid state" << endl;
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


std::shared_ptr<ThreadPool> gThreadPool;

void InitThreadPool(const unsigned int size)
{
    if(!gThreadPool)
    {
        gThreadPool.reset(new ThreadPool());
        gThreadPool->prepareNewThreads(size);
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



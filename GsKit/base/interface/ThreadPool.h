/*
 *  ThreadPool.h
 *  OpenLieroX
 *
 *  Created by Albert Zeyer on 08.02.09.
 *  code under LGPL
 *
 */

#ifndef THREADPOOL_H__
#define THREADPOOL_H__

#include <set>
#include <string>
#include <memory>

struct SDL_mutex;
struct SDL_cond;
struct SDL_Thread;
class ThreadPool;
typedef int (*ThreadFunc) (void*);
struct CmdLineIntf;

struct Action {
	virtual ~Action() {}
	virtual int handle() = 0;
};

struct ThreadPoolItem {
    SDL_Thread* thread = nullptr;
	std::string name;
    bool working = false;
    bool finished = false;
    bool headless = false;
    SDL_cond* finishedSignal = nullptr;
    SDL_cond* readyForNewWork = nullptr;
	int ret;
};

class ThreadPool {
private:
    SDL_mutex* mutex = nullptr;
    SDL_cond* awakeThread = nullptr;
    SDL_cond* threadStartedWork = nullptr;
    SDL_cond* threadStatusChanged = nullptr;
    Action* nextAction = nullptr;
    bool nextIsHeadless = false;
    std::string nextName;
    std::shared_ptr<ThreadPoolItem> nextData;
    bool quitting = false;
    std::set< std::shared_ptr<ThreadPoolItem> > availableThreads;
    std::set< std::shared_ptr<ThreadPoolItem> > usedThreads;

	void prepareNewThread();
	static int threadWrapper(void* param);
    SDL_mutex* startMutex = nullptr;
public:
    ThreadPool();
	~ThreadPool();

    void prepareNewThreads(const unsigned int size);

    std::shared_ptr<ThreadPoolItem>
    start(ThreadFunc fct, void* param = nullptr, const std::string& name = "unknown worker");

	// WARNING: if you set headless, you cannot use wait() and you should not save the returned ThreadPoolItem*
    std::shared_ptr<ThreadPoolItem>
    start(Action* act, const std::string& name = "unknown worker", const bool headless = false); // ThreadPool will own and free the Action

    bool finalizeIfReady(std::shared_ptr<ThreadPoolItem> &thread, int* status = nullptr);
    bool wait(ThreadPoolItem* thread, int* status = nullptr);
	bool waitAll();
	void dumpState(CmdLineIntf& cli) const;
};

extern std::shared_ptr<ThreadPool> gThreadPool;

void InitThreadPool(const unsigned int size = 5);
void UnInitThreadPool();



template<typename _T>
struct _ThreadFuncWrapper {
	typedef int (_T::* FuncPointer)();
	template< FuncPointer _func >
		struct Wrapper {
			static int wrapper(void* obj) {
				return (((_T*)obj) ->* _func)();
			}
			
            static std::shared_ptr<ThreadPoolItem> startThread(_T* const obj, const std::string& name) {
                return gThreadPool->start((ThreadFunc)&wrapper, (void*)obj, name);
			}
		};
};

#define StartMemberFuncInThread(T, memberfunc, name) \
_ThreadFuncWrapper<T>::Wrapper<&memberfunc>::startThread(this, name)


#endif // THREADPOOL_H__


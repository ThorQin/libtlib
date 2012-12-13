/*
 * tlibthread.h
 *
 *  Created on: 2010-9-8
 *      Author: thor
 */

#ifndef TLIBTHREAD_H_
#define TLIBTHREAD_H_

#ifdef __MSVC__
#include <windows.h>
#elif defined(__GNUC__)
#include <pthread.h>
#endif

namespace tlib {

class Mutex {
public:
	inline Mutex();
	inline ~Mutex();
	inline void lock();
	inline void unlock();
private:
#ifdef __MSVC__
	HANDLE _mutex;
#elif defined(__GNUC__)
	pthread_mutex_t _mutex;
#endif
};

inline Mutex::Mutex()
#ifdef __MSVC__
: _mutex(0)
#endif
{
#ifdef __MSVC__
	_mutex = CreateMutex(0, 0, 0);
#elif defined(__GNUC__)
	pthread_mutex_init(&_mutex, 0);
#endif
}

inline Mutex::~Mutex() {
#ifdef __MSVC__
	CloseHandle(_mutex);
#elif defined(__GNUC__)
	pthread_mutex_destroy(&_mutex);
#endif
}

inline void Mutex::lock() {
#ifdef __MSVC__
	WaitForSingleObject(_mutex, INFINITE);
#elif defined(__GNUC__)
	pthread_mutex_lock(&_mutex);
#else
#error not implement.
#endif
}

inline void Mutex::unlock() {
#ifdef __MSVC__
	ReleaseMutex(_mutex);
#elif defined(__GNUC__)
	pthread_mutex_unlock(&_mutex);
#endif
}

class Spin {
public:
	inline Spin();
	inline ~Spin();
	inline void lock();
	inline void unlock();
private:
#ifdef __MSVC__
	CRITICAL_SECTION _critical_section;
#elif defined(__GNUC__)
	pthread_spinlock_t _spin;
#else
#error not implement.
#endif
};

inline Spin::Spin() {
#ifdef __MSVC__
	InitializeCriticalSectionAndSpinCount(&_critical_section, 4000);
#elif defined(__GNUC__)
	pthread_spin_init(&_spin, PTHREAD_PROCESS_PRIVATE);
#else
#error not implement.
#endif
}
inline Spin::~Spin() {
#ifdef __MSVC__
	DeleteCriticalSection(&_critical_section);
#elif defined(__GNUC__)
	pthread_spin_destroy(&_spin);
#else
#error not implement.
#endif
}

inline void Spin::lock() {
#ifdef __MSVC__
	EnterCriticalSection(&_critical_section);
#elif defined(__GNUC__)
	pthread_spin_lock(&_spin);
#else
#error not implement.
#endif
}

inline void Spin::unlock() {
#ifdef __MSVC__
	LeaveCriticalSection(&_critical_section);
#elif defined(__GNUC__)
	pthread_spin_unlock(&_spin);
#else
#error not implement.
#endif
}

template<typename TRes>
class Lock {
public:
	inline Lock(TRes& res);
	inline ~Lock();
private:
	TRes& _res;
};

template<typename TRes>
inline Lock<TRes>::Lock(TRes& res) :
		_res(res) {
	_res.lock();
}
template<typename TRes>
inline Lock<TRes>::~Lock() {
	_res.unlock();
}

} // end of namespace tlib

#endif /* TLIBTHREAD_H_ */

#pragma once

#include <map>
#include <mutex>
#include <chrono>
#include <vector>
#include <string>
#include <atomic>
#include <iostream>

using std::map;
using std::list;
using std::cout;
using std::pair;
using std::mutex;
using std::atomic;
using std::vector;
using std::wstring;

template<typename T>
struct MyLock
{
	MyLock(int _iLockID)
		: iLockID(_iLockID) {}

	int			iLockID;
	T			tlock;
};

template<typename T>
class CMyLockGuard
{
public :
	CMyLockGuard(map< wstring, MyLock<T>>* _pMapLocks, const vector<wstring>& _vecRequestKeys)
		: pMapLocks(_pMapLocks)
	{
		if (pMapLocks)
		{
			for (auto requestKey : _vecRequestKeys)
			{
				auto iter = pMapLocks->find(requestKey);

				if (iter != pMapLocks->end())
				{
					vecLocks.push_back(&(iter->second));
				}
			}

			sort(vecLocks.begin(), vecLocks.end(), [](const auto& lhs, const auto& rhs) {
				return (lhs->iLockID < rhs->iLockID);
				});

			for (auto iter = vecLocks.begin(); iter != vecLocks.end(); ++iter)
			{
				(*iter)->tlock.lock();
				cout << "LOCK ID : " << (*iter)->iLockID << '\n';
			}
		}
	}

	~CMyLockGuard()
	{
		for (auto iter = vecLocks.rbegin(); iter != vecLocks.rend(); ++iter)
		{
			cout << "UNLOCK ID : " << (*iter)->iLockID << '\n';
			(*iter)->tlock.unlock();
		}
	}

public : 
	CMyLockGuard(const CMyLockGuard& _rhs)				= delete;
	CMyLockGuard& operator=(const CMyLockGuard& _rhs)	= delete;

private : 
	map< wstring, MyLock<T>>*		pMapLocks = { nullptr };
	vector<MyLock<T>*>				vecLocks;
};

class CMySpinLock
{
public :
	CMySpinLock()	= default;
	~CMySpinLock()	= default;

public :
	void lock() 
	{
		bool bExpected = { false };
		while (m_bLocked.compare_exchange_strong(bExpected, true) == false)
		{
			bExpected = false;

			std::this_thread::yield(); 
		}
	}
	
	void unlock() 
	{
		m_bLocked = false;
	}

private :
	atomic<bool>	m_bLocked = { false };
};

// 프로파일링을 위한 RAII 클래스
using MILLISECONDS	= std::chrono::milliseconds;
using TIMER			= std::chrono::high_resolution_clock;
using TIMER_MARK	= std::chrono::high_resolution_clock::time_point;

class CMyTimer
{
public :
	CMyTimer(list<pair<wstring, MILLISECONDS>>* _pTimers, const wstring& _wstrTimerTag)
		: m_pTimers(_pTimers), m_wstrTimerTag(_wstrTimerTag)
	{
		m_tStartPoint			= TIMER::now();
	}

	~CMyTimer()
	{
		m_tEndPoint				= TIMER::now();
		MILLISECONDS microsec	= std::chrono::duration_cast<std::chrono::milliseconds>(m_tEndPoint - m_tStartPoint);

		m_pTimers->push_back({ m_wstrTimerTag, microsec });
	}

private :
	wstring						m_wstrTimerTag;
	TIMER_MARK					m_tStartPoint;
	TIMER_MARK					m_tEndPoint;
	list<pair<wstring, MILLISECONDS>>* m_pTimers	= { nullptr };
};

template<typename T>
struct COUNTER
{
	T* ptr					= { nullptr };
	atomic<int> iRefCount	= { 0 };
	atomic<int> iWeakCount	= { 0 };

	COUNTER(T* _ptr)
		: ptr(_ptr) {}
	~COUNTER()
	{
		if (ptr)
		{
			delete ptr;
			ptr = nullptr;
		}
	}
};

template<typename T>
class CMyWeakPtr;

template<typename T>
class CMySharedPtr
{
	friend CMyWeakPtr<T>;

public:
	explicit CMySharedPtr(T* _ptr)
		: m_pCounter(new COUNTER<T>(_ptr)) {
		if (m_pCounter) m_pCounter->iRefCount.fetch_add(1);
	}
	explicit CMySharedPtr(COUNTER<T>* _ptr)
		: m_pCounter(_ptr) {
		if (m_pCounter) m_pCounter->iRefCount.fetch_add(1);
	}
	CMySharedPtr(const CMySharedPtr& _ptr)
		: m_pCounter(_ptr.m_pCounter) {
		if (m_pCounter) m_pCounter->iRefCount.fetch_add(1);
	}
	~CMySharedPtr()
	{
		Reset();
		cout << "~MySharedPtr()" << '\n';
	}

	T& operator*() { return *(m_pCounter->ptr); }
	const T& operator*()	const { return *(m_pCounter->ptr); }
	T* operator->() { return   m_pCounter->ptr; }

	void Reset()
	{
		if (m_pCounter == nullptr)
			return;

		m_pCounter->iRefCount.fetch_sub(1);

		if (m_pCounter->iRefCount == 0 && m_pCounter->iWeakCount == 0)
		{
			delete m_pCounter;
			m_pCounter = nullptr;
		}
		else if (m_pCounter->iRefCount == 0) // 객체만 삭제
		{
			delete m_pCounter->ptr;
			m_pCounter->ptr = nullptr;
		}
	}

private :
	COUNTER<T>* m_pCounter = { nullptr };
};

template<typename T>
class CMyWeakPtr
{
public:
	explicit CMyWeakPtr(const CMySharedPtr<T>& _ptr)
		: m_pCounter(_ptr.m_pCounter) { m_pCounter->iWeakCount.fetch_add(1); }
	~CMyWeakPtr()
	{
		Reset();
		cout << "~MyWeakPtr()" << '\n';
	}

public :
	CMySharedPtr<T> Lock()
	{
		return CMySharedPtr<T>(m_pCounter);
	}

private : 
	void Reset()
	{
		m_pCounter->iWeakCount.fetch_sub(1);

		if (m_pCounter->iRefCount == 0 && m_pCounter->iWeakCount == 0)
		{
			delete m_pCounter;
			m_pCounter = nullptr;
		}
	}

private :
	COUNTER<T>* m_pCounter = { nullptr };
};
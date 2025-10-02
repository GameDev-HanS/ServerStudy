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
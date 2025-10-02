#pragma once
#include "Server_Struct.h"

template<typename T>
class CMyStack
{
public :
	CMyStack();
	~CMyStack();

public :
	CMyStack(const CMyStack& _rhs)				= delete;
	CMyStack& operator=(const CMyStack& _rhs)	= delete;

public :
	void	push(T _data);
	T		pop();
	void	clear() { m_iSize = 0; }

public:
	bool	empty() const { return (m_iSize == 0); }
	T		top()	const { if (empty()) throw "Invalid operation"; return m_arr[0]; }

private:

	T*			m_arr		= { nullptr };
	int			m_iCapacity = { 10 };
	int			m_iSize		= { 0 };
	CMySpinLock	m_tLock;
};

template<typename T>
CMyStack<T>::CMyStack()
{
	m_arr = new T[m_iCapacity];
}

template<typename T>
CMyStack<T>::~CMyStack()
{
	delete[] m_arr;
	m_arr = nullptr;
}

template<typename T>
void CMyStack<T>::push(T _data)
{
	std::lock_guard<CMySpinLock> lock(m_tLock);

	cout << "Push : " << _data << '\n';

	if (m_iCapacity == m_iSize)
	{
		m_iCapacity *= 2;
		T* pNewArr = new T[m_iCapacity];

		for (int i = 0; i < m_iSize; ++i)
		{
			pNewArr[i] = m_arr[i];
		}

		delete[] m_arr;

		m_arr = pNewArr;
	}

	for (int i = m_iSize; i >= 1; --i) // O(N)
	{
		m_arr[i] = m_arr[i - 1];
	}

	m_arr[0] = _data;
	++m_iSize;
}

template<typename T>
T CMyStack<T>::pop()
{
	std::lock_guard<CMySpinLock> lock(m_tLock);

	if (empty())
	{
		cout << "Invalid operation\n";
		return -1;
	}

	T popElem = m_arr[0];

	for (int i = 0; i < m_iSize - 1; ++i) // O(N)
	{
		m_arr[i] = m_arr[i + 1];
	}

	--m_iSize;

	cout << "Pop : " << popElem << '\n';

	return popElem;
}
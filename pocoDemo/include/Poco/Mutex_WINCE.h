//
// Mutex_WIN32.h
//
// Library: Foundation
// Package: Threading
// Module:  Mutex
//
// Definition of the MutexImpl and FastMutexImpl classes for WIN32.
//
// Copyright (c) 2004-2010, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#ifndef Foundation_Mutex_WINCE_INCLUDED
#define Foundation_Mutex_WINCE_INCLUDED


#include "Poco/Foundation.h"
#include "Poco/Exception.h"
#include "Poco/UnWindows.h"


namespace Poco {


class Foundation_API MutexImpl
{
public:
	enum MutexTypeImpl
	{
		MUTEX_RECURSIVE_IMPL,
		MUTEX_NONRECURSIVE_IMPL,
	};

protected:
	explicit MutexImpl(MutexTypeImpl type);
	~MutexImpl();
	void lockImpl();
	bool tryLockImpl();
	bool tryLockImpl(long milliseconds);
	void unlockImpl();
	
private:
	HANDLE _mutex;
	int _lockCount;
	const bool _recursive;

private:
	MutexImpl(const MutexImpl&);
	MutexImpl& operator = (const MutexImpl&);
};


class Foundation_API FastMutexImpl
{
protected:
	FastMutexImpl();
	~FastMutexImpl();
	void lockImpl();
	bool tryLockImpl();
	bool tryLockImpl(long milliseconds);
	void unlockImpl();
	
private:
	HANDLE _mutex;
};


} // namespace Poco


#endif // Foundation_Mutex_WINCE_INCLUDED

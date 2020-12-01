// klist.h - Include file for template class KList, KInterlockedList
//=============================================================================
//
// Compuware Corporation
// NuMega Lab
// 9 Townsend West
// Nashua, NH 03063  USA
//
// Copyright (c) 2004 Compuware Corporation. All Rights Reserved.
// Unpublished - rights reserved under the Copyright laws of the
// United States.
//
// U.S. GOVERNMENT RIGHTS-Use, duplication, or disclosure by the
// U.S. Government is subject to restrictions as set forth in
// Compuware Corporation license agreement and as provided in
// DFARS 227.7202-1(a) and 227.7202-3(a) (1995),
// DFARS 252.227-7013(c)(1)(ii)(OCT 1988), FAR 12.212(a) (1995),
// FAR 52.227-19, or FAR 52.227-14 (ALT III), as applicable.
// Compuware Corporation.
//
// This product contains confidential information and trade secrets
// of Compuware Corporation. Use, disclosure, or reproduction is
// prohibited without the prior express written permission of Compuware
// Corporation.
//
//=============================================================================

#ifndef __KLIST__
#define __KLIST__

#include <list>
#include <mutex>

using namespace std;

#if defined(_AMD64_)
#define _RtlCompareMemory	RtlCompareMemory
#else
inline SIZE_T _RtlCompareMemory(const VOID *Source1, const VOID *Source2, SIZE_T Length)
{
	if (memcmp(Source1, Source2, Length) == 0)
		return Length;
	return 0;
}
#endif
//////////////////////////////////////////////////////////////////////
// _listcore (internal use only) This is a base class for all lists.
//
class _listcore
{

// Construction
protected:
//	SAFE_DESTRUCTORS
	_listcore(ULONG OffsetOfListEntry);
public:
	VOID Invalidate(void) {}
	BOOLEAN IsValid(){ return TRUE; }

// Methods
public:
	PVOID 	_head(void);
	PVOID 	_tail(void);
	PVOID 	_next(PVOID Item);
	PVOID 	_prev(PVOID Item);
	VOID 	_insertTail(PVOID Item);
	VOID 	_insertHead(PVOID Item);
	VOID 	_insertBefore(PVOID Item, PVOID BeforeThisItem);
	VOID 	_insertAfter(PVOID Item, PVOID AfterThisItem);
	PVOID 	_remove(PVOID Item);
	PVOID 	_removeTail(void);
	PVOID 	_removeHead(void);
	PVOID 	_find(PVOID Item, PVOID StartItem, BOOLEAN bForward);
	BOOLEAN	_isEmpty(void);
	ULONG 	_count(void);
	VOID 	_reset(void);
//	PVOID	Container(PLIST_ENTRY pLE);
//	PLIST_ENTRY ListEntryPointer(PVOID p);

// Data
protected:
//	LIST_ENTRY m_head;
//	ULONG_PTR m_ListEntryOffset;
};

//////////////////////////////////////////////////////////////////////
// KList
//
/*template <class T> class KList : protected _listcore
{

// Construction
public:
//	SAFE_DESTRUCTORS
	KList(void);
	KList(ULONG OffsetOfListEntry);

// Methods
public:
	T*		Head(void);
	T*		HeadNoLock(void);
	T* 		Tail(void);
	T* 		TailNoLock(void);
	T* 		Next(T* Item);
	T* 		NextNoLock(T* Item);
	T*		Prev(T* Item);
	T*		PrevNoLock(T* Item);
	VOID	InsertTail(T* Item);
	VOID	InsertTailNoLock(T* Item);
	VOID	InsertHead(T* Item);
	VOID	InsertHeadNoLock(T* Item);
	VOID 	InsertBefore(T* Item, T* BeforeThisItem);
	VOID 	InsertBeforeNoLock(T* Item, T* BeforeThisItem);
	VOID	InsertAfter(T* Item, T* AfterThisItem);
	VOID	InsertAfterNoLock(T* Item, T* AfterThisItem);
	T* 		Remove(T* Item);
	T* 		RemoveNoLock(T* Item);
	T* 		RemoveTail(void);
	T* 		RemoveTailNoLock(void);
	T* 		RemoveHead(void);
	T* 		RemoveHeadNoLock(void);
	T*		Find(T* Item, T* StartItem = NULL, BOOLEAN bForward = TRUE);
	T*		FindNoLock(T* Item, T* StartItem = NULL, BOOLEAN bForward = TRUE);
	BOOLEAN	IsEmpty(void);
	BOOLEAN	IsEmptyNoLock(void);
	ULONG 	Count(void);
	ULONG 	CountNoLock(void);
	VOID 	Reset(void);
	VOID 	ResetNoLock(void);
	virtual VOID Lock(void);
	virtual VOID Unlock(void);

//#ifndef VXD_COMPATLIB
#if (_WDM_ && (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x10))))
	T*		HeadAtDpc(void);
	T* 		TailAtDpc(void);
	T* 		NextAtDpc(T* Item);
	T*		PrevAtDpc(T* Item);
	VOID	InsertTailAtDpc(T* Item);
	VOID	InsertHeadAtDpc(T* Item);
	VOID 	InsertBeforeAtDpc(T* Item, T* BeforeThisItem);
	VOID	InsertAfterAtDpc(T* Item, T* AfterThisItem);
	T* 		RemoveAtDpc(T* Item);
	T* 		RemoveTailAtDpc(void);
	T* 		RemoveHeadAtDpc(void);
	T*		FindAtDpc(T* Item, T* StartItem = NULL, BOOLEAN bForward = TRUE);
	BOOLEAN	IsEmptyAtDpc(void);
	ULONG 	CountAtDpc(void);
	VOID 	ResetAtDpc(void);
	virtual VOID LockAtDpc(void);
	virtual VOID UnlockAtDpc(void);
//#endif
#endif

	using _listcore::IsValid;
	using _listcore::Invalidate;
protected:
	mutex	m_lock;
	list<T>	m_list;
};*/

//////////////////////////////////////////////////////////////////////
// KInterlockedList
//
template <class T> class KInterlockedList
{

// Construction
public:
//	SAFE_DESTRUCTORS
	KInterlockedList(void);
//	KInterlockedList(ULONG OffsetOfListEntry);

// Methods
public:
	T*		Head(void);
	T*		HeadNoLock(void);
	T* 		Tail(void);
	T* 		TailNoLock(void);
	T* 		Next(T* Item);
	T* 		NextNoLock(T* Item);
	T*		Prev(T* Item);
	T*		PrevNoLock(T* Item);
	VOID	InsertTail(T* Item);
	VOID	InsertTailNoLock(T* Item);
	VOID	InsertHead(T* Item);
	VOID	InsertHeadNoLock(T* Item);
//	VOID 	InsertBefore(T* Item, T* BeforeThisItem);
//	VOID 	InsertBeforeNoLock(T* Item, T* BeforeThisItem);
//	VOID	InsertAfter(T* Item, T* AfterThisItem);
//	VOID	InsertAfterNoLock(T* Item, T* AfterThisItem);
	T* 		Remove(T* Item);
	T* 		RemoveNoLock(T* Item);
	T* 		RemoveTail(void);
	T* 		RemoveTailNoLock(void);
	T* 		RemoveHead(void);
	T* 		RemoveHeadNoLock(void);
//	T*		Find(T* Item, T* StartItem = NULL, BOOLEAN bForward = TRUE);
//	T*		FindNoLock(T* Item, T* StartItem = NULL, BOOLEAN bForward = TRUE);
	BOOLEAN	IsEmpty(void);
	BOOLEAN	IsEmptyNoLock(void);
	ULONG 	Count(void);
	ULONG 	CountNoLock(void);
	VOID 	Reset(void);
	VOID 	ResetNoLock(void);
	VOID	Lock(void);
	VOID	Unlock(void);

//#ifndef VXD_COMPATLIB
#if (_WDM_ && (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x10))))
	T*		HeadAtDpc(void);
	T* 		TailAtDpc(void);
	T* 		NextAtDpc(T* Item);
	T*		PrevAtDpc(T* Item);
	VOID	InsertTailAtDpc(T* Item);
	VOID	InsertHeadAtDpc(T* Item);
	VOID 	InsertBeforeAtDpc(T* Item, T* BeforeThisItem);
	VOID	InsertAfterAtDpc(T* Item, T* AfterThisItem);
	T* 		RemoveAtDpc(T* Item);
	T* 		RemoveTailAtDpc(void);
	T* 		RemoveHeadAtDpc(void);
	T*		FindAtDpc(T* Item, T* StartItem = NULL, BOOLEAN bForward = TRUE);
	BOOLEAN	IsEmptyAtDpc(void);
	ULONG 	CountAtDpc(void);
	VOID 	ResetAtDpc(void);
	VOID	LockAtDpc(void);
	VOID	UnlockAtDpc(void);
//#endif
#endif

//	using _listcore::IsValid;
//	using _listcore::Invalidate;

// Data
protected:
	mutex	m_lock;
	list<T, allocator<T>>	m_list;
};
#if 0
#if !(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))
//////////////////////////////////////////////////////////////////////
// _isafelist (internal use only)
//
class _isafelist : protected _listcore
{

// Construction
protected:
	SAFE_DESTRUCTORS
	_isafelist(KInterrupt* pInterruptObject, ULONG OffsetOfListEntry);
	_isafelist(ULONG OffsetOfListEntry);
public:
	VOID Initialize(KInterrupt* pInterruptObject);
	VOID Invalidate(void){ m_intobj = NULL; }
	BOOLEAN IsValid(void){ return (m_intobj != NULL); }

// Methods
protected:
	static BOOLEAN _synchHead(PVOID Context);
	static BOOLEAN _synchTail(PVOID Context);
	static BOOLEAN _synchNext(PVOID Context);
	static BOOLEAN _synchPrev(PVOID Context);
	static BOOLEAN _synchInsertTail(PVOID Context);
	static BOOLEAN _synchInsertHead(PVOID Context);
	static BOOLEAN _synchInsertBefore(PVOID Context);
	static BOOLEAN _synchInsertAfter(PVOID Context);
	static BOOLEAN _synchRemoveHead(PVOID Context);
	static BOOLEAN _synchRemoveTail(PVOID Context);
	static BOOLEAN _synchRemove(PVOID Context);
	static BOOLEAN _synchFind(PVOID Context);
	static BOOLEAN _synchIsEmpty(PVOID Context);
	static BOOLEAN _synchCount(PVOID Context);
	static BOOLEAN _synchReset(PVOID Context);
	static VOID Synch(PKSYNCHRONIZE_ROUTINE pSynchFunc, ...);

// Data
protected:
	KInterrupt* m_intobj;
};

//////////////////////////////////////////////////////////////////////
// KInterruptSafeList
//
template <class T> class KInterruptSafeList : public _isafelist
{

// Construction
public:
	SAFE_DESTRUCTORS
	KInterruptSafeList(KInterrupt* pInterruptObject);
	KInterruptSafeList(KInterrupt* pInterruptObject, ULONG OffsetOfListEntry);
	KInterruptSafeList(void);
	KInterruptSafeList(ULONG OffsetOfListEntry);

// Methods
public:

	// Users of the class should set AtDirql to TRUE (constant)
	// when calling from ISRs or synch crit section routines. This
	// enables the compiler to generate optimal code that bypasses
	// the synchronization logic.
	T*		Head(BOOLEAN AtDirql=FALSE);
	T*		Tail(BOOLEAN AtDirql=FALSE);
	T*		Next(T* Item, BOOLEAN AtDirql=FALSE);
	T*		Prev(T* Item, BOOLEAN AtDirql=FALSE);
	VOID	InsertTail(T* Item, BOOLEAN AtDirql=FALSE);
	VOID 	InsertHead(T* Item, BOOLEAN AtDirql=FALSE);
	VOID	InsertBefore(T* Item, T* BeforeThisItem, BOOLEAN AtDirql=FALSE);
	VOID	InsertAfter(T* Item, T* AfterThisItem, BOOLEAN AtDirql=FALSE);
	T* 		RemoveHead(BOOLEAN AtDirql=FALSE);
	T*		RemoveTail(BOOLEAN AtDirql=FALSE);
	T*		Remove(T* Item, BOOLEAN AtDirql=FALSE);
	T*		Find(T* Item, BOOLEAN AtDirql = FALSE, T* StartItem = NULL,
		BOOLEAN bForward = TRUE);
	BOOLEAN	IsEmpty(BOOLEAN AtDirql=FALSE);
	ULONG	Count(BOOLEAN AtDirql=FALSE);
	VOID	Reset(BOOLEAN AtDirql=FALSE);
};
#endif //!(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))
#endif
////////////////////////////////////////////////////////////////////////
// Generic offset determinator for type T
/*template <class T> inline ULONG_PTR ListEntryOffset(T* p)
{
	return (ULONG_PTR) &((T*)0)->m_ListEntry;
	UNREFERENCED_PARAMETER(p);
}
////////////////////////////////////////////////////////////////////////

template <class T> KList<T>::KList(ULONG OffsetOfListEntry) :
	_listcore(OffsetOfListEntry)
{
}

template <class T> KList<T>::KList(void) :
	_listcore(0)
{
}*/
/*
////////////////////////////////////////////////////////////////////////
inline PLIST_ENTRY _listcore::ListEntryPointer(PVOID p)
{
	return  (PLIST_ENTRY)((ULONG_PTR)p + m_ListEntryOffset);
}
////////////////////////////////////////////////////////////////////////
inline PVOID _listcore::Container(PLIST_ENTRY pLE)
{
	if (pLE == &m_head)
		return NULL;
	else
		return (PVOID)( (PCHAR)pLE - m_ListEntryOffset );
}
*/
////////////////////////////////////////////////////////////////////////
/*template <class T> inline T* KList<T>::Head(void)
{
	Lock();
	T* pT = (T*)_head();
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::HeadNoLock(void)
{
	return (T*)_head();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::Tail(void)
{
	Lock();
	T* pT = (T*)_tail();
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::TailNoLock(void)
{
	return (T*)_tail();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertTail(T* Item)
{
	Lock();
	_insertTail(Item);
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertTailNoLock(T* Item)
{
	_insertTail(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertHead(T* Item)
{
	Lock();
	_insertHead(Item);
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertHeadNoLock(T* Item)
{
	_insertHead(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertBefore(T* Item, T* BeforeThisItem)
{
	Lock();
	_insertBefore(Item, BeforeThisItem);
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertBeforeNoLock(T* Item, T* BeforeThisItem)
{
	_insertBefore(Item, BeforeThisItem);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertAfter(T* Item, T* AfterThisItem)
{
	Lock();
	_insertAfter(Item, AfterThisItem);
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertAfterNoLock(T* Item, T* AfterThisItem)
{
	_insertAfter(Item, AfterThisItem);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::Next(T* Item)
{
	Lock();
	T* pT = (T*)_next(Item);
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::NextNoLock(T* Item)
{
	return (T*)_next(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::Prev(T* Item)
{
	Lock();
	T* pT = (T*)_prev(Item);
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::PrevNoLock(T* Item)
{
	return (T*)_prev(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::Remove(T* Item)
{
	Lock();
	T* pT = (T*)_remove(Item);
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveNoLock(T* Item)
{
	return (T*)_remove(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline BOOLEAN KList<T>::IsEmpty(void)
{
	Lock();
	BOOLEAN empty = _isEmpty();
	Unlock();
	return empty;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline BOOLEAN KList<T>::IsEmptyNoLock(void)
{
	return _isEmpty();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveTail(void)
{
	Lock();
	T* pT = (T*)_removeTail();
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveTailNoLock(void)
{
	return (T*)_removeTail();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveHead(void)
{
	Lock();
	T* pT = (T*)_removeHead();
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveHeadNoLock(void)
{
	return (T*)_removeHead();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::Find(T* Item, T* StartItem,
	BOOLEAN bForward)
{
	Lock();
	T* pT = (T*)_find(Item, StartItem, bForward);
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::FindNoLock(T* Item, T* StartItem,
	BOOLEAN bForward)
{
	return (T*)_find(Item, StartItem, bForward);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline ULONG KList<T>::Count(void)
{
	Lock();
	ULONG count = _count();
	Unlock();
	return count;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline ULONG KList<T>::CountNoLock(void)
{
	return _count();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::Reset(void)
{
	Lock();
	_reset();
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::ResetNoLock(void)
{
	_reset();
}
////////////////////////////////////////////////////////////////////////
template <class T> VOID KList<T>::Lock(void)
{
	m_lock.lock();
}
////////////////////////////////////////////////////////////////////////
template <class T> VOID KList<T>::Unlock(void)
{
	m_lock.unlock();
}

//#ifndef VXD_COMPATLIB
#if (_WDM_ && (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x10))))
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::HeadAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_head();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::TailAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_tail();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertTailAtDpc(T* Item)
{
	LockAtDpc();
	_insertTail(Item);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertHeadAtDpc(T* Item)
{
	LockAtDpc();
	_insertHead(Item);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertBeforeAtDpc(T* Item, T* BeforeThisItem)
{
	LockAtDpc();
	_insertBefore(Item, BeforeThisItem);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::InsertAfterAtDpc(T* Item, T* AfterThisItem)
{
	LockAtDpc();
	_insertAfter(Item, AfterThisItem);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::NextAtDpc(T* Item)
{
	LockAtDpc();
	T* pT = (T*)_next(Item);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::PrevAtDpc(T* Item)
{
	LockAtDpc();
	T* pT = (T*)_prev(Item);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveAtDpc(T* Item)
{
	LockAtDpc();
	T* pT = (T*)_remove(Item);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline BOOLEAN KList<T>::IsEmptyAtDpc(void)
{
	LockAtDpc();
	BOOLEAN empty = _isEmpty();
	UnlockAtDpc();
	return empty;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveTailAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_removeTail();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::RemoveHeadAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_removeHead();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KList<T>::FindAtDpc(T* Item, T* StartItem,
	BOOLEAN bForward)
{
	LockAtDpc();
	T* pT = (T*)_find(Item, StartItem, bForward);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline ULONG KList<T>::CountAtDpc(void)
{
	LockAtDpc();
	ULONG count = _count();
	UnlockAtDpc();
	return count;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KList<T>::ResetAtDpc(void)
{
	LockAtDpc();
	_reset();
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> VOID KList<T>::LockAtDpc(void)
{
}
////////////////////////////////////////////////////////////////////////
template <class T> VOID KList<T>::UnlockAtDpc(void)
{
}
//#endif
#endif
*/
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
template <class T> KInterlockedList<T>::KInterlockedList(void)
{
}
////////////////////////////////////////////////////////////////////////
/*template <class T> KInterlockedList<T>::KInterlockedList(ULONG OffsetOfListEntry) :
	_listcore(OffsetOfListEntry)
{
}*/
////////////////////////////////////////////////////////////////////////
template <class T> VOID KInterlockedList<T>::Lock(void)
{
	m_lock.lock();
}
////////////////////////////////////////////////////////////////////////
template <class T> VOID KInterlockedList<T>::Unlock(void)
{
	m_lock.unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::Head(void)
{
	lock_guard<mutex>	lck(m_lock);
	T* pT = (T*)&m_list.front();	// _head();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::HeadNoLock(void)
{
	return (T*)&m_list.front();	// _head();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::Tail(void)
{
	lock_guard<mutex>	lck(m_lock);
	T* pT = (T*)&m_list.back();	// _tail();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::TailNoLock(void)
{
	return (T*)&m_list.back();	// _tail();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertTail(T* Item)
{
	lock_guard<mutex>	lck(m_lock);
	m_list.push_back(*Item);	// _insertTail(Item);
	free( Item );
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertTailNoLock(T* Item)
{
	m_list.push_back(*Item);	// _insertTail(Item);
	free( Item );
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertHead(T* Item)
{
	lock_guard<mutex>	lck(m_lock);
	m_list.push_front(*Item);	// _insertHead(Item);
	free( Item );
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertHeadNoLock(T* Item)
{
	m_list.push_front(*Item);	// _insertHead(Item);
	free( Item );
}
////////////////////////////////////////////////////////////////////////
/*template <class T> inline VOID KInterlockedList<T>::InsertBefore(T* Item, T* BeforeThisItem)
{
	Lock();
	_insertBefore(Item, BeforeThisItem);
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertBeforeNoLock(T* Item, T* BeforeThisItem)
{
	_insertBefore(Item, BeforeThisItem);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertAfter(T* Item, T* AfterThisItem)
{
	Lock();
	m_list.insert(AfterThisItem, *Item);	// _insertAfter(Item, AfterThisItem);
	Unlock();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertAfterNoLock(T* Item, T* AfterThisItem)
{
	m_list.insert(AfterThisItem, *Item);	//_insertAfter(Item, AfterThisItem);
}*/
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::Next(T* Item)
{
	T	*pT = NULL;
	try
	{
		lock_guard<mutex>	lck(m_lock);
		for (class list<T>::iterator it = m_list.begin(); it != m_list.end(); it++)
		{
			if (_RtlCompareMemory(Item, &(*it), sizeof(T)) == sizeof(T))
			{
				it++;
				if (it != m_list.end()) pT = (T*) &(*it);

				break;
			}
		}
	}
	catch (system_error& e)
	{
		printf("KInterlockedList Next. e=%s", e.code().message().c_str());
	}
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::NextNoLock(T* Item)
{
	T	*pT = NULL;
	for (class list<T>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if (_RtlCompareMemory(Item, &(*it), sizeof(T)) == sizeof(T))
		{
			it++;
			if (it != m_list.end()) pT = (T*) &(*it);

			break;
		}
	}
	return pT;	// _next(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::Prev(T* Item)
{
	T	*pT = NULL;
	lock_guard<mutex>	lck(m_lock);

	for (class list<T>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if (_RtlCompareMemory(Item, &(*it), sizeof(T)) == sizeof(T))
		{
			if (it != m_list.begin())
			{
				it--;
				pT = (T*) &(*it);
			}
			break;
		}
	}
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::PrevNoLock(T* Item)
{
	T	*pT = NULL;
	for (class list<T>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if (_RtlCompareMemory(Item, &(*it), sizeof(T)) == sizeof(T))
		{
			if (it != m_list.begin())
			{
				it--;
				pT = (T*) &(*it);
			}
			break;
		}
	}
	return pT;	// _prev(Item);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::Remove(T* Item)
{
	T	*pT = Next(Item);
	try
	{
		lock_guard<mutex>	lck(m_lock);
		for (class list<T>::iterator it = m_list.begin(); it != m_list.end(); it++)
		{
			if (_RtlCompareMemory(Item, &(*it), sizeof(T)) == sizeof(T))
			{
				m_list.erase(it);
				break;
			}
		}
	}
	catch (system_error& e)
	{
		printf("KInterlockedList Remove. e=%s", e.code().message().c_str());
	}
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveNoLock(T* Item)
{
	T	*pT = NextNoLock(Item);
	for (class list<T>::iterator it = m_list.begin(); it != m_list.end(); it++)
	{
		if (_RtlCompareMemory(Item, &(*it), sizeof(T)) == sizeof(T))
		{
			m_list.erase(it);
			break;
		}
	}
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline BOOLEAN KInterlockedList<T>::IsEmpty(void)
{
	lock_guard<mutex>	lck(m_lock);
	BOOLEAN empty = m_list.empty();	//_isEmpty();
	return empty;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline BOOLEAN KInterlockedList<T>::IsEmptyNoLock(void)
{
	return m_list.empty();	//_isEmpty();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveTail(void)
{
	T* pT = new T;
	lock_guard<mutex>	lck(m_lock);
	RtlCopyMemory(pT, (T*)&m_list.back(), sizeof(T));	// _removeTail();
	m_list.pop_back();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveTailNoLock(void)
{
	T* pT = new T;
	RtlCopyMemory(pT, (T*)&m_list.back(), sizeof(T));	// _removeTail();
	m_list.pop_back();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveHead(void)
{
	T* pT = new T;
	lock_guard<mutex>	lck(m_lock);
	RtlCopyMemory( pT, (T*)&m_list.front(), sizeof(T) );	// _removeHead();
	m_list.pop_front();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveHeadNoLock(void)
{
	T* pT = new T;
	RtlCopyMemory(pT, (T*)&m_list.front(), sizeof(T));	// _removeHead();
	m_list.pop_front();
	return pT;
}
////////////////////////////////////////////////////////////////////////
/*template <class T> inline T* KInterlockedList<T>::Find(T* Item, T* StartItem,
	BOOLEAN bForward)
{
	Lock();
	T* pT = (T*)_find(Item, StartItem, bForward);
	Unlock();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::FindNoLock(T* Item, T* StartItem,
	BOOLEAN bForward)
{
	return (T*)_find(Item, StartItem, bForward);
}*/
////////////////////////////////////////////////////////////////////////
template <class T> inline ULONG KInterlockedList<T>::Count(void)
{
	lock_guard<mutex>	lck(m_lock);
	ULONG count = m_list.size();	// _count();
	return count;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline ULONG KInterlockedList<T>::CountNoLock(void)
{
	return m_list.size();	// _count();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::Reset(void)
{
	lock_guard<mutex>	lck(m_lock);
	m_list.clear();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::ResetNoLock(void)
{
	m_list.clear();
}

//#ifndef VXD_COMPATLIB
#if (_WDM_ && (WDM_MAJORVERSION > 1 ||((WDM_MAJORVERSION == 1) && (WDM_MINORVERSION >= 0x10))))
////////////////////////////////////////////////////////////////////////
template <class T> VOID KInterlockedList<T>::LockAtDpc(void)
{
}
////////////////////////////////////////////////////////////////////////
template <class T> VOID KInterlockedList<T>::UnlockAtDpc(void)
{
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::HeadAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_head();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::TailAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_tail();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertTailAtDpc(T* Item)
{
	LockAtDpc();
	_insertTail(Item);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertHeadAtDpc(T* Item)
{
	LockAtDpc();
	_insertHead(Item);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertBeforeAtDpc(T* Item, T* BeforeThisItem)
{
	LockAtDpc();
	_insertBefore(Item, BeforeThisItem);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::InsertAfterAtDpc(T* Item, T* AfterThisItem)
{
	LockAtDpc();
	_insertAfter(Item, AfterThisItem);
	UnlockAtDpc();
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::NextAtDpc(T* Item)
{
	LockAtDpc();
	T* pT = (T*)_next(Item);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::PrevAtDpc(T* Item)
{
	LockAtDpc();
	T* pT = (T*)_prev(Item);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveAtDpc(T* Item)
{
	LockAtDpc();
	T* pT = (T*)_remove(Item);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline BOOLEAN KInterlockedList<T>::IsEmptyAtDpc(void)
{
	LockAtDpc();
	BOOLEAN empty = _isEmpty();
	UnlockAtDpc();
	return empty;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveTailAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_removeTail();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::RemoveHeadAtDpc(void)
{
	LockAtDpc();
	T* pT = (T*)_removeHead();
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline T* KInterlockedList<T>::FindAtDpc(T* Item, T* StartItem,
	BOOLEAN bForward)
{
	LockAtDpc();
	T* pT = (T*)_find(Item, StartItem, bForward);
	UnlockAtDpc();
	return pT;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline ULONG KInterlockedList<T>::CountAtDpc(void)
{
	LockAtDpc();
	ULONG count = _count();
	UnlockAtDpc();
	return count;
}
////////////////////////////////////////////////////////////////////////
template <class T> inline VOID KInterlockedList<T>::ResetAtDpc(void)
{
	LockAtDpc();
	_reset();
	UnlockAtDpc();
}
//#endif
#endif
#if 0
////////////////////////////////////////////////////////////////////////
#if !(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))

template <class T> inline
	KInterruptSafeList<T>::KInterruptSafeList(KInterrupt* pInterruptObject) :
	_isafelist(pInterruptObject, ListEntryOffset((T*)0))
{
}

template <class T> inline
	KInterruptSafeList<T>::KInterruptSafeList(
		KInterrupt* pInterruptObject,
		ULONG OffsetOfListEntry) :
	_isafelist(pInterruptObject, OffsetOfListEntry)
{
}

template <class T> inline
	KInterruptSafeList<T>::KInterruptSafeList(void) :
	_isafelist(ListEntryOffset((T*)0))
{
}

template <class T> inline
	KInterruptSafeList<T>::KInterruptSafeList( ULONG OffsetOfListEntry) :
	_isafelist(OffsetOfListEntry)
{
}
////////////////////////////////////////////////////////////////////////
//
// In the following functions, parameter AtDirql is usually either
// defaulted to constant FALSE, or specified as constant TRUE. In a
// free build, the compiler will optimize out the "if" statement.
//
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /

template <class T>	inline
	BOOLEAN KInterruptSafeList<T>::IsEmpty(BOOLEAN AtDirql)
{
	if (AtDirql)
		return _isEmpty();
	else
	{
		BOOLEAN bReturn;
		Synch(
			_synchIsEmpty,
			this,
			&bReturn
			);
		return bReturn;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	VOID KInterruptSafeList<T>::InsertTail(T* Item, BOOLEAN AtDirql)
{
	if (AtDirql)
		_insertTail(Item);
	else
		Synch(
			_synchInsertTail,
			this,
			Item
			);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	VOID KInterruptSafeList<T>::InsertHead(T* Item, BOOLEAN AtDirql)
{
	if (AtDirql)
		_insertHead(Item);
	else
		Synch(
			_synchInsertHead,
			this,
			Item
			);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	VOID KInterruptSafeList<T>::InsertBefore(T* Item, T* BeforeThisItem, BOOLEAN AtDirql)
{
	if (AtDirql)
		_insertBefore(Item, BeforeThisItem);
	else
		Synch(
			_synchInsertBefore,
			this,
			Item,
			BeforeThisItem
			);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	VOID KInterruptSafeList<T>::InsertAfter(T* Item, T* AfterThisItem, BOOLEAN AtDirql)
{
	if (AtDirql)
		_insertAfter(Item, AfterThisItem);
	else
		Synch(
			_synchInsertAfter,
			this,
			Item,
			AfterThisItem
			);
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::Head(BOOLEAN AtDirql)
{
	if (AtDirql)
		return (T*)_head();
	else
	{
		T* h;
		Synch(_synchHead, this,	&h);
		return h;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::Tail(BOOLEAN AtDirql)
{
	if (AtDirql)
		return (T*)_tail();
	else
	{
		T* t;
		Synch(_synchTail, this, &t);
		return t;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::RemoveHead(BOOLEAN AtDirql)
{
	if (AtDirql)
		return (T*)_removeHead();
	else
	{
		T* r;
		Synch(_synchRemoveHead, this, &r);
		return r;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::RemoveTail(BOOLEAN AtDirql)
{
	if (AtDirql)
		return (T*)_removeTail();
	else
	{
		T* r;
		Synch(_synchRemoveTail, this, &r);
		return r;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::Next(T* Item, BOOLEAN AtDirql)
{
	if (AtDirql)
		return (T*)_next(Item);
	else
	{
		T* n;
		Synch(_synchNext, this, Item, &n);
		return n;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::Prev(T* Item, BOOLEAN AtDirql)
{
	if (AtDirql)
		return (T*)_prev(Item);
	else
	{
		T* p;
		Synch(_synchPrev, this, Item, &p);
		return p;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::Find(T* Item, BOOLEAN AtDirql, T* StartItem,
	BOOLEAN bForward)
{
	if (AtDirql)
		return (T*)_find(Item, StartItem, bForward);
	else
	{
		T* n;
		Synch(_synchFind, this, Item, StartItem, bForward, &n);
		return n;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	T* KInterruptSafeList<T>::Remove(T* Item, BOOLEAN AtDirql)
{
	if (AtDirql)
		return (T*)_remove(Item);
	else
	{
		Synch(_synchRemove, this, Item);
		return Item;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T>	inline
	ULONG KInterruptSafeList<T>::Count(BOOLEAN AtDirql)
{
	if (AtDirql)
		return _count();
	else
	{
		ULONG c;
		Synch(_synchCount, this, &c);
		return c;
	}
}
////////////////////////////////////////////////////////////////////////
template <class T> inline
	VOID KInterruptSafeList<T>::Reset(BOOLEAN AtDirql)
{
	if (AtDirql)
		return _reset();
	else
		Synch(_synchReset, this);
}
#endif //!(!_WDM_ && (defined(_IA64_) || defined(_AMD64_)))
#endif

/*#ifndef VXD_COMPATLIB
////////////////////////////////////////////////////////////////////////
// Offset determinator for IRP lists
inline ULONG_PTR ListEntryOffset(IRP* Irp)
{
	return (ULONG_PTR)(&((IRP*)0)->Tail.Overlay.ListEntry);
}
#endif*/

#endif

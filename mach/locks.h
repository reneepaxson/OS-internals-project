// //
// //  Implementation of synchronization reader/writer locks
// //
// //  dennisbabkin.com
// //

// #ifndef locks_h
// #define locks_h

// #include <pthread.h>
// #include <sched.h>
// #include <exception>


// ///Class that implements a reader/writer lock's base
// ///
// struct RDR_WRTR
// {
// 	RDR_WRTR()
// 	{
// 	}
	
// 	///RETURN:
// 	///     - 0 if success
// 	///     - Otherwise it's an error, for instance: EDEADLK
// 	int EnterReaderLock()
// 	{
// 		return pthread_rwlock_rdlock(&_lock);
// 	}
	
// 	void LeaveReaderLock()
// 	{
// 		int nErr = pthread_rwlock_unlock(&_lock);
// 		if(nErr != 0)
// 		{
// 			//Logical problem - need to crash
// 			assert(false);
// 			abort();
// 		}
// 	}
	
// 	///RETURN:
// 	///     - true if lock was not available for reading (NOTE that it may be now!)
// 	///     - false if lock was available for reading (NOTE that it may not be anymore!)
// 	bool WasReaderLocked()
// 	{
// 		//Try to acquire it for reading, but do not block
// 		int nErr = pthread_rwlock_tryrdlock(&_lock);
// 		if(nErr == 0)
// 		{
// 			//Acquired it, need to release it
// 			nErr = pthread_rwlock_unlock(&_lock);
// 			if(nErr == 0)
// 			{
// 				return false;
// 			}
// 			else
// 			{
// 				//Logical problem - need to crash
// 				assert(false);
// 				abort();
// 			}
// 		}
// 		else if(nErr == EBUSY)
// 		{
// 			//Not available
// 		}
// 		else
// 		{
// 			//Logical problem - need to crash
// 			assert(false);
// 			abort();
// 		}

// 		return true;
// 	}
	
	
// 	///RETURN:
// 	///     - 0 if success
// 	///     - Otherwise it's an error, for instance: EDEADLK
// 	int EnterWriterLock()
// 	{
// 		return pthread_rwlock_wrlock(&_lock);
// 	}
	
// 	void LeaveWriterLock()
// 	{
// 		int nErr = pthread_rwlock_unlock(&_lock);
// 		if(nErr != 0)
// 		{
// 			//Logical problem - need to crash
// 			assert(false);
// 			abort();
// 		}
// 	}

	
// 	///RETURN:
// 	///     - true if lock was not available for writing (NOTE that it may be now!)
// 	///     - false if lock was available for writing (NOTE that it may not be anymore!)
// 	bool WasWriterLocked()
// 	{
// 		//Try to acquire it for reading, but do not block
// 		int nErr = pthread_rwlock_trywrlock(&_lock);
// 		if(nErr == 0)
// 		{
// 			//Acquired it, need to release it
// 			nErr = pthread_rwlock_unlock(&_lock);
// 			if(nErr == 0)
// 			{
// 				return false;
// 			}
// 			else
// 			{
// 				//Logical problem - need to crash
// 				assert(false);
// 				abort();
// 			}
// 		}
// 		else if(nErr == EBUSY)
// 		{
// 			//Not available
// 		}
// 		else
// 		{
// 			//Logical problem - need to crash
// 			assert(false);
// 			abort();
// 		}

// 		return true;
// 	}
	
	

	
// private:
// 	//Copy constructor and assignments are NOT available!
// 	RDR_WRTR(const RDR_WRTR& s) = delete;
// 	RDR_WRTR& operator = (const RDR_WRTR& s) = delete;
	
	
// private:
// 	pthread_rwlock_t _lock = PTHREAD_RWLOCK_INITIALIZER;
// };




// ///Class that implements a reader-lock
// //INFO: This lock will block only if a writer-lock is currently holding it.
// //
// struct READER_LOCK
// {
// 	READER_LOCK(RDR_WRTR& rwl)
// 		: _rwl(rwl)
// 	{
// 		int nErr = rwl.EnterReaderLock();
// 		if(nErr != 0)
// 		{
// 			//Logical problem - need to crash
// 			assert(false);
// 			abort();
// 		}
// 	}

// 	~READER_LOCK()
// 	{
// 		_rwl.LeaveReaderLock();
// 	}

	
// private:
// 	//Copy constructor and assignments are NOT available!
// 	READER_LOCK(const READER_LOCK& s) = delete;
// 	READER_LOCK& operator = (const READER_LOCK& s) = delete;
	
// private:
// 	RDR_WRTR& _rwl;
// };





// ///Class that implements a writer-lock
// //INFO: This lock will block if a reader- or a writer-lock is currently holding it.
// //
// struct WRITER_LOCK
// {
// 	WRITER_LOCK(RDR_WRTR& rwl)
// 		: _rwl(rwl)
// 	{
// 		int nErr = rwl.EnterWriterLock();
// 		if(nErr != 0)
// 		{
// 			//Logical problem - need to crash
// 			assert(false);
// 			abort();
// 		}
// 	}

// 	~WRITER_LOCK()
// 	{
// 		_rwl.LeaveWriterLock();
// 	}

// private:
// 	//Copy constructor and assignments are NOT available!
// 	WRITER_LOCK(const WRITER_LOCK& s) = delete;
// 	WRITER_LOCK& operator = (const WRITER_LOCK& s) = delete;
	
// private:
// 	RDR_WRTR& _rwl;
// };
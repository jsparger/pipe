//---------------------------------------------------------------------------//
// Copyright (c) 2014 John Sparger <jsparger87@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See https://github.com/jsparger/pipe for more information.
//---------------------------------------------------------------------------//

#ifndef PIPE_MODULE_HH
#define PIPE_MODULE_HH

#include "pipe/MessageBundle.hh"
#include "pipe/ControlMessage.hh"
#include "pipe/BundleAccess.hh"
#include <memory>
#include <condition_variable>
#include <mutex>

namespace pipe {

class Module
{
public:
	Module()
		: 	next(0), newDataReady(false), isAlive(true), lock(m), 
			dataLock(dataMutex), bundle(new MessageBundle)
	{
		// initial state is:
		// no module connected.
		// we do not have new data ready for us to process
		// the mutex is locked.
		// bundle is null
		// data is invalid (so unlocked)
		dataLock.unlock();
	}
	
	virtual ~Module() {;}
	
	virtual void push(std::unique_ptr<MessageBundle> newBundle)
	{	
		// don't accept the data
		if (false == isAlive)
		{
			return;
		}
	
		// this method is called by an external thread, so let's
		// describe it from the external module's (Module A's) perspective:
		{
			// Wait until Module B is waiting to accept new data.
			// (We can only acquire a lock on the mutex when Module B is
			// waiting)
			// Also, wait for data to be unlocked. This means the data has been
			// processed and is now trash. 
			std::lock_guard<std::mutex> plock(dataMutex);			
			std::lock_guard<std::mutex> tmplock(m);
					
			// Module B has already passed on the bundle it was working with.
			// The contents of "bundle" are now trash. Swap them for the fresh
			// and minty newBundle
			bundle.swap(newBundle);
		
			// Set the check variable "newDataReady" to true.
			newDataReady = true;
		}
		
		// lock the data
		dataLock.lock();
	
		// Notify the waiting thread (module B) that we have given it new data.
		cv.notify_one();
	}
	
	virtual Module& connect(Module& module)
	{
		// connect the module
		next = &module;
		return module;	
	}
	
	virtual void operator()(bool persist = false)
	{	
		initialize();
	
		do
		{
			waitForData();
			processControlMessage();
			processData();
			pushData();		
		
		} while (persist && isAlive);
	
		cleanUp();
	}
	
protected:
	virtual void waitForData()
	{
		// wait to be notified that new data is ready. Lock is released while
		// we are in the waiting state and re-acquired as soon as we wake up.
		// We will confirm that we were not spuriously awakened by checking
		// for newDataReady == true.
		cv.wait(lock, [this]{return this->newDataReady;});
		// set the newDataReady flag to false.
		newDataReady = false;
	}
	
	virtual void processControlMessage()
	{
		ControlMessage m;
		if (false == controlAccess.readFrom(bundle, m)) { return; }
	
		if (ControlMessage::Type::SOFT_RESET == m.type)
		{
			reset();
		}
		else if (ControlMessage::Type::SHUTDOWN == m.type)
		{
			shutDown();
		}
	}
	
	virtual void shutDown()
	{
		// change our alive status to false.
		isAlive = false;
	}
	
	virtual void processData() = 0;
	
	virtual void pushData()
	{
		// push the bundle on to the next module in the chain.
		// contents of bundle are now trash.
		if (next) { next->push(std::move(bundle)); }
	
		// unlock the data
		dataLock.unlock();
	}
	
	virtual void initialize()
	{
		// do nothing
	}
	
	virtual void cleanUp()
	{
		// unlock mutex to allow any thread currently blocked
		// pushing data to us to complete.
		lock.unlock();
	}
	
	virtual void reset()
	{
		// do nothing
	}
	
	Module* next;
	bool newDataReady;
	bool isAlive;
	std::mutex m, dataMutex;
	std::unique_lock<std::mutex> lock;
	std::unique_lock<std::mutex> dataLock;
	std::condition_variable cv;
	std::unique_ptr<MessageBundle> bundle;
	BundleAccess<ControlMessage> controlAccess;
};
	
} // namespace pipe

#endif
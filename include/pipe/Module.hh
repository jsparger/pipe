//---------------------------------------------------------------------------//
// Copyright (c) 2014 John Sparger <jsparger87@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See https://github.com/jsparger/pipe for more information.
//---------------------------------------------------------------------------//

/// @file Module.hh
/// @brief dogtime all the way

#ifndef PIPE_MODULE_HH
#define PIPE_MODULE_HH

#include "pipe/MessageBundle.hh"
#include "pipe/ControlMessage.hh"
#include "pipe/BundleAccess.hh"
#include <memory>
#include <condition_variable>
#include <mutex>

namespace pipe {

/// \class Module
/// \brief Abstract base class for modules in an analysis pipeline.
///
/// The Module class is the abstract base class for modules in analysis pipeline. To create a new Module, the user needs only to inherit from Module and provide the processData() member function. The Module base class handles receiving and passing data to other modules in a thread safe way, as well as inspecting bundles for control messages. Several other member functions can be overriden if the user requires access to different parts of the module's operational cycle.

class Module
{
public:
	/// Constructor. Initializes module to safe state.
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
	
	/// Destructor
	virtual ~Module() {;}
	
	/// called by previous module in chain to give this module its message bundle.
	virtual void push(std::unique_ptr<MessageBundle> newBundle)
	{	
		// don't accept the data
		if (false == isAlive)
		{
			return;
		}
	
		// this member function is called by an external thread, so let's
		// describe it from the external module's (Module A's) perspective:
		{
			// Wait until Module B is waiting to accept new data.
			// (We can only acquire a lock on the mutex when Module B is
			// waiting) This requires locking on m.
			// Also, wait for data to be unlocked. This means the data has been
			// processed and is now trash. 
			std::lock_guard<std::mutex> wantsDataLock(dataMutex);			
			std::lock_guard<std::mutex> isWaitingLock(m);
					
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
	
	/// Connects a module to this module. Returns the newly connected module to enable chained calls.
	virtual Module& connect(Module& module)
	{
		// connect the module
		next = &module;
		return module;	
	}
	
	/// Starts the module's operation. If \p persist is false the module will only process one cycle
	virtual void operator()(bool persist = true)
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
	
	/// This member function is called after the current message bundle has been processed. It blocks until a fresh message bundle has been pushed to the module.
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
	
	/// Processes control messages to trigger various callbacks such as reset() and shutDown().
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
	
	/// Called when a shutdown control message is received.
	virtual void shutDown()
	{
		// change our alive status to false.
		isAlive = false;
	}
	
	/// This member function should be implemented to define the functionality of user modules. This member function is called during every cycle of the module to process the message bundle that was received. The user can use this hook to inspect the message bundle, do work with its contents, and attach new data.
	virtual void processData() = 0;
	
	/// Pushes data to the next module in the chain and allows new data to be pushed to this module
	virtual void pushData()
	{
		// push the bundle on to the next module in the chain.
		// contents of bundle are now trash.
		if (next) { next->push(std::move(bundle)); }
	
		// unlock the data
		dataLock.unlock();
	}
	
	/// A hook to perform any post-constructor initialization the module might need.
	virtual void initialize()
	{
		// do nothing
	}
	
	/// A hook to perform any cleanup a module might need to do before execution is shutdown. This is the last function called on the last cycle of execution. If this function is overridden, the base class version (this version) should also be invoked for proper behavior.
	virtual void cleanUp()
	{
		// unlock mutex to allow any thread currently blocked
		// pushing data to us to complete.
		lock.unlock();
	}
	
	/// A hook which is called whenever a soft-reset controll message is received.
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
	/// The message bundle. To be accessed directly by user modules in their processData() implementation.
	std::unique_ptr<MessageBundle> bundle;
	BundleAccess<ControlMessage> controlAccess;
};
	
} // namespace pipe

#endif
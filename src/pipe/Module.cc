// //---------------------------------------------------------------------------//
// // Copyright (c) 2014 John Sparger <jsparger87@gmail.com>
// //
// // Distributed under the Boost Software License, Version 1.0
// // See accompanying file LICENSE_1_0.txt or copy at
// // http://www.boost.org/LICENSE_1_0.txt
// //
// // See https://github.com/jsparger/pipe for more information.
// //---------------------------------------------------------------------------//
//
// #include "utkVme/pipe/Module.hh"
// #include <iostream>
//
// namespace utkVme {
// namespace pipe {
//
// Module::
// Module()
// 	: 	next(0), newDataReady(false), isAlive(true), lock(m),
// 		dataLock(dataMutex), bundle(new MessageBundle)
// {
// 	// initial state is:
// 	// no module connected.
// 	// we do not have new data ready for us to process
// 	// the mutex is locked.
// 	// bundle is null
// 	// data is invalid (so unlocked)
// 	dataLock.unlock();
// }
//
// void
// Module::
// push(std::unique_ptr<MessageBundle> newBundle)
// {
// 	// don't accept the data
// 	if (false == isAlive)
// 	{
// 		return;
// 	}
//
// 	// this method is called by an external thread, so let's
// 	// describe it from the external module's (Module A's) perspective:
// 	{
// 		// Wait until Module B is waiting to accept new data.
// 		// (We can only acquire a lock on the mutex when Module B is
// 		// waiting)
// 		// Also, wait for data to be unlocked. This means the data has been
// 		// processed and is now trash.
// 		std::lock_guard<std::mutex> plock(dataMutex);
// 		std::lock_guard<std::mutex> tmplock(m);
//
// 		// Module B has already passed on the bundle it was working with.
// 		// The contents of "bundle" are now trash. Swap them for the fresh
// 		// and minty newBundle
// 		bundle.swap(newBundle);
//
// 		// Set the check variable "newDataReady" to true.
// 		newDataReady = true;
// 	}
//
// 	// lock the data
// 	dataLock.lock();
//
// 	// Notify the waiting thread (module B) that we have given it new data.
// 	cv.notify_one();
// }
//
// Module&
// Module::
// connect(Module& module)
// {
// 	// connect the module
// 	next = &module;
// 	return module;
// }
//
// void
// Module::
// operator()(bool persist)
// {
// 	initialize();
//
// 	do
// 	{
// 		waitForData();
// 		processControlMessage();
// 		processData();
// 		pushData();
//
// 	} while (persist && isAlive);
//
// 	cleanUp();
// }
//
// void
// Module::
// waitForData()
// {
// 	// wait to be notified that new data is ready. Lock is released while
// 	// we are in the waiting state and re-acquired as soon as we wake up.
// 	// We will confirm that we were not spuriously awakened by checking
// 	// for newDataReady == true.
// 	cv.wait(lock, [this]{return this->newDataReady;});
// 	// set the newDataReady flag to false.
// 	newDataReady = false;
// }
//
// void
// Module::
// processControlMessage()
// {
// 	ControlMessage m;
// 	if (false == controlAccess.readFrom(bundle, m)) { return; }
//
// 	if (ControlMessage::Type::SOFT_RESET == m.type)
// 	{
// 		reset();
// 	}
// 	else if (ControlMessage::Type::SHUTDOWN == m.type)
// 	{
// 		shutDown();
// 	}
// }
//
// void
// Module::
// shutDown()
// {
// 	// change our alive status to false.
// 	isAlive = false;
// }
//
// void
// Module::
// reset()
// {
// 	// do nothing
// }
//
// void
// Module::
// pushData()
// {
// 	// push the bundle on to the next module in the chain.
// 	// contents of bundle are now trash.
// 	if (next) { next->push(std::move(bundle)); }
//
// 	// unlock the data
// 	dataLock.unlock();
// }
//
// void
// Module::
// initialize()
// {
// 	// do nothing
// }
//
// void
// Module::
// cleanUp()
// {
// 	// do nothing
//
// 	// unlock mutex to allow any thread currently blocked
// 	// pushing data to us to complete.
// 	lock.unlock();
// }
//
// } // namespace pipe
// } // namespace utkVme
//
//
//
//
//
//
//
//
//
//
//
//
//
//

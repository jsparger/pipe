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
// #include "utkVme/pipe/Pipeline.hh"
// #include "utkVme/pipe/Interrupt.hh"
//
// namespace utkVme {
// namespace pipe {
//
// Pipeline::
// Pipeline()
// : 	terminateSignal(false)
// {
// 	// we will not be receiving any data on startup.
// 	// our empty bundle is good enough though. Lock the dataLock.
// 	dataLock.lock();
// }
//
// Module&
// Pipeline::
// connect(Module& m)
// {
//
// 	if (moduleVec.empty())
// 	{
// 		// if this is the first module added to the pipeline,
// 		// connect directly to it.
// 		Module::connect(m);
// 	}
// 	else
// 	{
// 		// otherwise...
// 		// connect the module to the last module added to
// 		// the pipeline.
// 		moduleVec.back()->connect(m);
// 	}
//
// 	// store a pointer to the module
// 	moduleVec.push_back(&m);
//
// 	// return *this for chaining calls to this method
// 	return *this;
// }
//
// Pipeline&
// Pipeline::
// connect(std::unique_ptr<Module> m)
// {
// 	// in this case, tbe lifetime of the module m will
// 	// be tied to the lifetime of the pipeline. store the
// 	// unique_ptr and connect as usual.
// 	ownedModuleVec.push_back(std::move(m));
// 	return static_cast<Pipeline&>(connect(*ownedModuleVec.back()));
// }
//
// void
// Pipeline::
// processData()
// {
// 	// we are connected to the last module in the chain, so the
// 	// data we receive is an old bundle we generated, though
// 	// perhaps now containing interrupts from the modules in the
// 	// pipeline.
//
// 	// swap a fresh bundle in for the "end of line" bundle we have now.
// 	std::unique_ptr<MessageBundle> endOfLine(new MessageBundle);
// 	bundle.swap(endOfLine);
//
// 	// inspect the end of line bundle and take action if need be.
// 	if (terminateSignal)
// 	{
// 		BundleAccess<ControlMessage> controlAccess;
// 		ControlMessage cm(ControlMessage::Type::SHUTDOWN);
// 		if(false == controlAccess.attachTo(bundle, cm))
// 		{
// 			std::cerr << "Pipeline::processData, already a control "
// 							"message in bundle somehow!\n";
// 		}
// 	}
// 	else
// 	{
// 		processEndOfLine(endOfLine);
// 	}
// }
//
// void
// Pipeline::
// processEndOfLine(std::unique_ptr<MessageBundle>& eol)
// {
// 	// check for an interrupt
// 	BundleAccess<Interrupt> interruptAccess;
// 	Interrupt interrupt;
// 	if (false == interruptAccess.readFrom(eol, interrupt))
// 	{
// 		return;
// 	}
//
// 	// TODO: This should atually evaluate the Interrupt
// 	// if we found one, treat it as shutdown for now.
// 	BundleAccess<ControlMessage> controlAccess;
// 	ControlMessage cm;
//
// 	if (Interrupt::Type::BREAK_POINT == interrupt.type)
// 	{
// 		cm.type = ControlMessage::Type::SOFT_RESET;
// 	}
// 	else if (Interrupt::Type::FINISHED == interrupt.type)
// 	{
// 		cm.type = ControlMessage::Type::SHUTDOWN;
// 	}
//
// 	if(false == controlAccess.attachTo(bundle, cm))
// 	{
// 		std::cerr << "Pipeline::processEndOfLine, already a control "
// 						"message in bundle somehow!\n";
// 	}
// }
//
// void
// Pipeline::
// operator()(bool persist)
// {
// 	// Check to see if any modules are attached.
// 	// If there are no modules, we have nothing to do, so return.
// 	if (moduleVec.empty())
// 	{
// 		return;
// 	}
//
// 	// connect the pipeline to the last module in the chain.
// 	// This forms a ring so we can receive feedback from the modules.
// 	moduleVec.back()->connect(*this);
//
// 	// launch the modules, each in a separate thread.
// 	for (auto module : moduleVec)
// 	{
// 		threadVec.push_back(
// 			std::thread(std::bind(std::ref(*module), persist))
// 			);
// 	}
//
// 	// Run a loop very similar to the normal one from Module, except we will
// 	// skip the initial waitForData (we will be providing the first bundle)
// 	// and skip the initial processControlMessages (we inject all control messages
// 	// so we know there won't be any)
// 	processData();
// 	pushData();
// 	while (persist && isAlive)
// 	{
// 		waitForData();
// 		processControlMessage();
// 		processData();
// 		pushData();
// 	}
// 	cleanUp();
//
// 	// join all all module threads
// 	for (auto& thread : threadVec)
// 	{
// 		thread.join();
// 	}
// }
//
// void
// Pipeline::
// terminate()
// {
// 	// to be called externally. Forces shutdown signal to be sent.
// 	// terminateSignal is atomic, so this is threadsafe.
// 	terminateSignal = true;
// }
//
// // void
// // Pipeline::
// // cleanUp()
// // {
// // 	// unlock the lock so the last module in the chain trying to push can complete.
// // 	lock.unlock();
// // }
//
//
//
// } // namespace pipe
// } // namespace utkVme
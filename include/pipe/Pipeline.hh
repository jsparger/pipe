//---------------------------------------------------------------------------//
// Copyright (c) 2014 John Sparger <jsparger87@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See https://github.com/jsparger/pipe for more information.
//---------------------------------------------------------------------------//

#ifndef PIPE_PIPELINE_HH
#define PIPE_PIPELINE_HH

#include <thread>
#include <vector>
#include "pipe/Module.hh"
#include <atomic>

namespace pipe {

/// \class Pipeline
/// \brief A special Module that goes at the beginning of a an analysis pipeline.
///
/// Pipeline is a special module that takes care of launching the modules in a chain, generating the bundles, and handling interrupts to terminate execution in the pipeline. Every group of modules should start with a Pipeline module.

class Pipeline : public Module
{
public:
	/// Constructor.
	Pipeline()
		: 	terminateSignal(false)
	{
		// we will not be receiving any data on startup.
		// our empty bundle is good enough though. Lock the dataLock.
		dataLock.lock();
	}
	
	/// Destructor
	virtual ~Pipeline() {;}
	
	/// connect a module to the end of the pipeline. This has a different chaining mechanism than calling connect on another module. Calling connect on a Module in the chain will not have the same effect as calling connect on the Pipeline.
	virtual Module& connect(Module& m) override
	{

		if (moduleVec.empty())
		{ 
			// if this is the first module added to the pipeline,
			// connect directly to it.
			Module::connect(m);
		}
		else
		{
			// otherwise...
			// connect the module to the last module added to
			// the pipeline.
			moduleVec.back()->connect(m);
		}
	
		// store a pointer to the module
		moduleVec.push_back(&m);
	
		// return *this for chaining calls to this method
		return *this;
	}
	
	// // not an override <-- the chaining doesn't really work
	// virtual Pipeline& connect(std::unique_ptr<Module> m)
	// {
	// 	//TODO: this whole method looks wrong.
	// 	// in this case, tbe lifetime of the module m will
	// 	// be tied to the lifetime of the pipeline. store the
	// 	// unique_ptr and connect as usual.
	// 	ownedModuleVec.push_back(std::move(m));
	// 	// TODO: this looks wrong
	// 	return static_cast<Pipeline&>(connect(*ownedModuleVec.back()));
	// }
	
	/// Starts the pipeline's operation. This will launch each module in its own thread and begin this module (Pipeline's) own operational cycle.
	virtual void operator()(bool persist = true) override
	{
		// Check to see if any modules are attached.
		// If there are no modules, we have nothing to do, so return.
		if (moduleVec.empty())
		{
			return;
		}
	
		// connect the pipeline to the last module in the chain.
		// This forms a ring so we can receive feedback from the modules.
		moduleVec.back()->connect(*this);
	
		// launch the modules, each in a separate thread.
		for (auto module : moduleVec)
		{
			threadVec.push_back(
				std::thread(std::bind(std::ref(*module), persist))
				);
		}
		
		// Run a loop very similar to the normal one from Module, except we will
		// skip the initial waitForData (we will be providing the first bundle)
		// and skip the initial processControlMessages (we inject all control messages
		// so we know there won't be any)
		processData();
		pushData();
		while (persist && isAlive)
		{
			waitForData();
			processControlMessage();
			processData();
			pushData();
		} 
		cleanUp();
	
		// join all all module threads
		for (auto& thread : threadVec)
		{
			thread.join();
		}	
	}
	
	/// Call this method externally to force a shutdown signal to be sent through the pipeline. This method is threadsafe.
	virtual void terminate()
	{
		// to be called externally. Forces shutdown signal to be sent.
		// terminateSignal is atomic, so this is threadsafe.
		terminateSignal = true;	
	} 
	
protected:
	
	/// Provides a new MessageBundle to the pipeline, checks for Interrupt messages, checks for termination, and provides ControlMessages.
	virtual void processData() override
	{
		// we are connected to the last module in the chain, so the
		// data we receive is an old bundle we generated, though
		// perhaps now containing interrupts from the modules in the 
		// pipeline.
	
		// swap a fresh bundle in for the "end of line" bundle we have now.
		std::unique_ptr<MessageBundle> endOfLine(new MessageBundle);
		bundle.swap(endOfLine);
	
		// inspect the end of line bundle and take action if need be.
		if (terminateSignal)
		{
			BundleAccess<ControlMessage> controlAccess;
			ControlMessage cm(ControlMessage::Type::SHUTDOWN);
			if(false == controlAccess.attachTo(bundle, cm))
			{
				// TODO: throw error instead	
				std::cerr << "Pipeline::processData, already a control " 
								"message in bundle somehow!\n";
			}
		}
		else
		{
			processEndOfLine(endOfLine);
		}
	}
	
	///  Helper function that hecks for Interrupts and writes the corresponding ControlMessage to the MessageBundle.
	virtual void processEndOfLine(std::unique_ptr<MessageBundle>& eol)
	{
		// check for an interrupt
		BundleAccess<Interrupt> interruptAccess;
		Interrupt interrupt;
		if (false == interruptAccess.readFrom(eol, interrupt))
		{
			return;
		}
	
		// TODO: This should atually evaluate the Interrupt
		// if we found one, treat it as shutdown for now.
		BundleAccess<ControlMessage> controlAccess;
		ControlMessage cm;
	
		if (Interrupt::Type::BREAK_POINT == interrupt.type)
		{
			cm.type = ControlMessage::Type::SOFT_RESET;
		}
		else if (Interrupt::Type::FINISHED == interrupt.type)
		{
			cm.type = ControlMessage::Type::SHUTDOWN;
		}
	
		if(false == controlAccess.attachTo(bundle, cm))
		{
			// TODO: throw error instead
			std::cerr << "Pipeline::processEndOfLine, already a control " 
							"message in bundle somehow!\n";
		}
	}
	
	std::vector<Module*> moduleVec;
	std::vector<std::unique_ptr<Module>> ownedModuleVec;
	std::vector<std::thread> threadVec;
	std::atomic<bool> terminateSignal;
};
	
} // namespace pipe

#endif
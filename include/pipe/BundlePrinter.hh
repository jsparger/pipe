//---------------------------------------------------------------------------//
// Copyright (c) 2014 John Sparger <jsparger87@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See https://github.com/jsparger/pipe for more information.
//---------------------------------------------------------------------------//

#ifndef PIPE_BUNDLE_PRINTER_HH
#define PIPE_BUNDLE_PRINTER_HH

#include "utkVme/pipe/Module.hh"
#include "utkVme/pipe/BundleAccess.hh"

namespace pipe {

// VERSION A:
// this class will only be instantiated when the parameter pack
// is empty due to the existence of the specialization that splits
// a parameter off from the pack as "class T". 
template <class... Args>
struct BundlePrinter 
{
	bool printAll;
	bool printedSomething;
	
	BundlePrinter(bool printAlways=false, bool printedSomething=false)
	:	printAll(printAlways),printedSomething(printedSomething)
	{
		// do nothing else;
	}
	
	void print(std::unique_ptr<utkVme::pipe::MessageBundle>& b)
	{
		// parameter pack is empty
		if (printedSomething)
		{
			std::cout << "=======================\n";
		}
	}
};

// VERSION B:
// This specialization will be used until the parameter pack is empty.
// It prints the message if it exists and then compile-time recursively calls
// "itself" until the parameter pack is empty, at which point the unspecialized
// version (Version A) above is called.
template <class T, class... Args>
struct BundlePrinter<T,Args...>
{		

	bool printAll, printedSomething;
	
	BundlePrinter(bool printAlways=false, bool printedSomething=false)
	:	printAll(printAlways), printedSomething(false)
	{
		// do nothing else;
	}
	
	void print(std::unique_ptr<utkVme::pipe::MessageBundle>& b)
	{
		utkVme::pipe::BundleAccess<T> access;
		T message;
		
		if(!b)
		{
			std::cout << "NULL BUNDLE!\n";
			return;
		}
		else if(false == access.readFrom(b, message) )
		{
			if(printAll)
			{
				std::cout <<  T::MessageType << ": no message!\n";
				printedSomething = true; 
			}
		}
		else
		{
			std::cout << T::MessageType << ":\n"<< message << "\n";
			printedSomething = true; 
		}
		
		BundlePrinter<Args...> theRest(printAll,printedSomething);
		theRest.print(b);
	}
};


template <class... Args>
class BundlePrintModule : public Module
{
public:
	BundlePrintModule(bool printAlways = false) 
		: printAll(printAlways)
		{
			// do nothing else;
		}
	virtual ~BundlePrintModule() {;}
	
protected:
	bool printAll;

	virtual void processData() override
	{
		BundlePrinter<Args...> bp(printAll);
		bp.print(bundle);

	}	
};
	
} // namespace pipe	

#endif

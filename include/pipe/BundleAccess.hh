//---------------------------------------------------------------------------//
// Copyright (c) 2014 John Sparger <jsparger87@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See https://github.com/jsparger/pipe for more information.
//---------------------------------------------------------------------------//

#ifndef PIPE_BUNDLE_ACCESS_HH
#define PIPE_BUNDLE_ACCESS_HH

#include "pipe/MessageBundle.hh"
#include <iostream>
#include <memory>

namespace pipe {

template <class T> 
class BundleAccess : public MessageBundle::Accessor
{
public:
	virtual ~BundleAccess() {;}

	// get the message of type T from the bundle if it exists (check first with inspect).
	// will throw error if message does not exist.
	virtual bool readFrom(std::unique_ptr<MessageBundle>& bundle, T& message)
	{
		// get the map from the bundle
		MessageBundle::MessageMap& map = getMap(*bundle.get());
		
		// check to see if bundle contains message of type T, if not, return false.
		if (map.count(T::MessageType) == 0) { return false; }
		
		// get the message. Store the unwrapped object in "message" and signal success.
		boost::any wrapped = map[T::MessageType];
		message = boost::any_cast<T>(wrapped);
		return true;
	}
	
	// I just don't like this. It seems dangerous. Should probably delete this method.
	virtual const T& readRef(std::unique_ptr<MessageBundle>& bundle)
	{
		// get the map from the bundle
		MessageBundle::MessageMap& map = getMap(*bundle.get());
		
		// check to see if bundle contains message of type T, if not, throw error
		// TODO: Throw a real exception!
		if (map.count(T::MessageType) == 0) { std::cerr << "BundleAccess::readRef(...): Error! No message of requested type!"; std::exit(0); }
		
		// get the message.
		boost::any& wrapped = map[T::MessageType];
		
		// http://www.boost.org/doc/libs/1_42_0/doc/html/boost/any_cast.html
		return *boost::any_cast<T>(&wrapped);
	}
	
	// attach the message of type T to the bundle. Returns false if message of type
	// T has already been attached to bundle
	virtual bool attachTo(std::unique_ptr<MessageBundle>& bundle, const T& message)
	{
			// get the map from the bundle
			MessageBundle::MessageMap& map = getMap(*bundle.get());

			// check to see if bundle alrady contains message of type T. 
			// if so, return false to indicate failure.
			if (map.count(T::MessageType) == 1) { return false; }
			
			map[T::MessageType] = boost::any(message);
			
			return true;
	}
	
	virtual bool hasMessage(std::unique_ptr<MessageBundle>& bundle)
	{
		// get the map from the bundle
		MessageBundle::MessageMap& map = getMap(*bundle.get());
		return (map.find(T::MessageType) != map.end());
	}
};
	

template <class T> 
class BundleAccessByName : public MessageBundle::Accessor
{
public:
	virtual ~BundleAccessByName() {;}	
	
	virtual bool checkFor(std::unique_ptr<MessageBundle>& bundle, const std::string& name)
	{
		auto& map = getMap(*bundle);
		auto it = map.find(name);
		return (map.end() != it);
	}
	
	virtual const T& readRef(std::unique_ptr<MessageBundle>& bundle, const std::string& name)
	{
		auto& map = getMap(*bundle);
		auto it = map.find(name);
		if (map.end() == it) { std::cerr << "BundleAccess::readRef(...): Error! No message of requested type!"; std::exit(0); }
		boost::any& theAny = it->second;
		T* ret = boost::any_cast<T>(&theAny);
		return *ret;
	}
	
	virtual bool attachTo(std::unique_ptr<MessageBundle>& bundle, const T& message, const std::string& name)
	{
		// get the map from the bundle
		auto& map = getMap(*bundle);
		auto pair = map.emplace(name, boost::any(message));
		std::cout << "insert = " << pair.second << "\n";
		return pair.second;
	}
};

} // namespace pipe

#endif
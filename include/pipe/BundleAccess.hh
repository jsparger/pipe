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

//TODO: What about move semantics? I thought I fixed this?

/// \class BundleAccess
/// \brief Used to write/read data from a MessageBundle with naming determined by message type.
///
/// BundleAccess will allow you to write/read data of type \p T to/from a MessageBundle. BundleAccess relies on a static method T::GetMessageType() to name the data. This allows you to retrieve data based on the type of data. However, this means there can only be one instance of a given data type in the map. To retrieve abritrarily named data from the MessageBundle, use BundleAccessByName.
template <class T> 
class BundleAccess : public MessageBundle::Accessor
{
public:
	
	/// Destructor
	virtual ~BundleAccess() {;}

	/// Get the message of type T from the bundle if it exists (check first with inspect). Will return false if message does not exist. Will throw exception if unpacking type erased data fails.
	virtual bool readFrom(std::unique_ptr<MessageBundle>& bundle, T& message)
	{
		// get the map from the bundle
		MessageBundle::MessageMap& map = getMap(*bundle.get());
		
		// check to see if bundle contains message of type T, if not, return false.
		if (map.count(T::GetMessageType()) == 0) { return false; }
		
		// get the message. Store the unwrapped object in "message" and signal success.
		boost::any wrapped = map[T::GetMessageType()];
		message = boost::any_cast<T>(wrapped);
		return true;
	}
	
	/// I just don't like this. It seems dangerous. Should probably delete this method.
	virtual const T& readRef(std::unique_ptr<MessageBundle>& bundle)
	{
		// get the map from the bundle
		MessageBundle::MessageMap& map = getMap(*bundle.get());
		
		// check to see if bundle contains message of type T, if not, throw error
		// TODO: Throw a real exception!
		if (map.count(T::GetMessageType()) == 0) { std::cerr << "BundleAccess::readRef(...): Error! No message of requested type!"; std::exit(0); }
		
		// get the message.
		boost::any& wrapped = map[T::GetMessageType()];
		
		// http://www.boost.org/doc/libs/1_42_0/doc/html/boost/any_cast.html
		return *boost::any_cast<T>(&wrapped);
	}
	
	/// Attach the message of type T to the bundle. Returns false if message of type T has already been attached to bundle
	virtual bool attachTo(std::unique_ptr<MessageBundle>& bundle, const T& message)
	{
			// get the map from the bundle
			MessageBundle::MessageMap& map = getMap(*bundle.get());

			// check to see if bundle alrady contains message of type T. 
			// if so, return false to indicate failure.
			if (map.count(T::GetMessageType()) == 1) { return false; }
			
			map[T::GetMessageType()] = boost::any(message);
			
			return true;
	}
	
	/// Check to see if a message of type \p T exists in the bundle.
	virtual bool hasMessage(std::unique_ptr<MessageBundle>& bundle)
	{
		// get the map from the bundle
		MessageBundle::MessageMap& map = getMap(*bundle.get());
		return (map.find(T::GetMessageType()) != map.end());
	}
};
	
/// \class BundleAccessByName
/// \brief Used to write/read data from a MessageBundle with arbitrary naming.
///
/// BundleAccess will allow you to write/read data of type \p T to/from a MessageBundle using an arbitrary name.
template <class T> 
class BundleAccessByName : public MessageBundle::Accessor
{
public:
	/// Destructor
	virtual ~BundleAccessByName() {;}	
	
	/// check to see if data associated with the string \p name exists in the bundle.
	virtual bool checkFor(std::unique_ptr<MessageBundle>& bundle, const std::string& name)
	{
		auto& map = getMap(*bundle);
		auto it = map.find(name);
		return (map.end() != it);
	}
	
	/// Retrieve a constant reference to the data associated with the string \p name from the bundle. Will throw an error if the string is not associated with any message or if unpacking the data to type \p T fails.
	virtual const T& readRef(std::unique_ptr<MessageBundle>& bundle, const std::string& name)
	{
		auto& map = getMap(*bundle);
		auto it = map.find(name);
		// TODO: throw a real error.
		if (map.end() == it) { std::cerr << "BundleAccess::readRef(...): Error! No message of requested type!"; std::exit(0); }
		boost::any& theAny = it->second;
		T* ret = boost::any_cast<T>(&theAny);
		return *ret;
	}
	
	/// Attach the data in \p message to the bundle and associate it with the string \name.
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
//---------------------------------------------------------------------------//
// Copyright (c) 2014 John Sparger <jsparger87@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See https://github.com/jsparger/pipe for more information.
//---------------------------------------------------------------------------//

#ifndef PIPE_MESSAGE_BUNDLE_HH
#define PIPE_MESSAGE_BUNDLE_HH

#include <map>
#include <string>
#include <boost/any.hpp>

namespace pipe {

/// \class MessageBundle
/// \brief Class used to pass data between modules.
///
/// The MessageBundle class is used to pass data between modules. Internally it maps pieces of data to a name, which can be used to retrieve the data from the MessageBundle later. Access to the message bundle contents is handled by an Accessor. The provided accessors BundleAccess and BundleAccessByName should cover most use cases, but the user is free to implement their own accessor if needed.

class MessageBundle
{
public:
	MessageBundle() {;}
	typedef std::map<std::string,boost::any> MessageMap;
	
private:
	/// A type erased map for storing data.
	MessageMap map;
	friend class Accessor;
	
public:
	
	/// \class Accessor
	/// \brief Provides access to the contents of a MessageBundle.
	///
	/// The Accessor inner class will provide access to the contents of a MessageBundle. It is desirable to hide the type erasure of boost::any from user code, so this hurdle was added. Inherit from this class to gain the ability to access the guts of a bundle.
	class Accessor
	{
	public:
		/// Destructor
		virtual ~Accessor() {;}
	protected:
		/// Get the type erased map from the MessageBundle.
		virtual MessageMap& getMap(MessageBundle& b) final
		{
			return b.map;
		}
	};
};

} // namespace pipe

#endif

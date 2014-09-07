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

class MessageBundle
{
public:
	MessageBundle() {;}
	typedef std::map<std::string,boost::any> MessageMap;
private:

	MessageMap map;
	friend class Accessor;
public:
	// this inner class (automatically a friend) will provide access
	// to the contents of the message bundle. They shouldn't really
	// be messed with in their boost::any state by any user code, so
	// this means you really have to try to access them. Inherit from
	// this class to gain the ability to access the guts of a bundle.
	class Accessor
	{
	public:
		virtual ~Accessor() {;}
	protected:
		virtual MessageMap& getMap(MessageBundle& b) final
		{
			return b.map;
		}
	};
};

} // namespace pipe

#endif

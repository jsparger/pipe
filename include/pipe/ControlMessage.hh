//---------------------------------------------------------------------------//
// Copyright (c) 2014 John Sparger <jsparger87@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See https://github.com/jsparger/pipe for more information.
//---------------------------------------------------------------------------//

#ifndef PIPE_CONTROL_MESSAGE_HH
#define PIPE_CONTROL_MESSAGE_HH

#include "pipe/Message.hh"
#include <string>
#include <type_traits>

namespace pipe {

struct ControlMessage : public Message
{
	enum class Type
	{
		NORMAL,
		SHUTDOWN,
		SOFT_RESET
	};
	
	static const std::string MessageType;
	Type type;
	
	ControlMessage(Type t = Type::NORMAL)
		: 	type(t) 
	{ 
		// do nothing else
	}
	
	virtual ~ControlMessage() {;}
	
	virtual void serialize(std::ostream& os) const override
	{
		os << MessageType << ": type = " << static_cast<std::underlying_type<Type>::type>(type);
	}
};


} // namespace pipe

#endif
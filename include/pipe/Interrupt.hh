//---------------------------------------------------------------------------//
// Copyright (c) 2014 John Sparger <jsparger87@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See https://github.com/jsparger/pipe for more information.
//---------------------------------------------------------------------------//

#ifndef PIPE_INTERRUPT_HH
#define PIPE_INTERRUPT_HH

#include "pipe/Message.hh"

namespace pipe {

struct Interrupt : public Message
{
	enum class Type
	{
		BREAK_POINT,
		FINISHED
	};
	
	Interrupt(Type t = Type::FINISHED)
		: 	type(t) 
	{ 
		// do nothing else
	}
	
	virtual ~Interrupt() {;}
	
	virtual void serialize(std::ostream& os) const override
	{
		os << MessageType << ": type = " << static_cast<std::underlying_type<Type>::type>(type);
	}
	
	const static std::string MessageType;
	Type type;
};
	
} // namespace pipe

#endif
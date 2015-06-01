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

namespace piper {

/// \class Interrupt
/// \brief A Message used by a module to signal state to the controller
///
/// The Interrupt is a type of Message which a module can attach to a MessageBundle. When the controller of the pipeline sees the interrupt, it can trigger an appropriate response. An interrupt should be set by a module in the pipeline.

struct Interrupt : public Message
{
	/// Possible types for the Interrupt
	enum class Type
	{
		BREAK_POINT, ///< The module has reached a break point in processing.
		FINISHED ///< The module is done with all its work.
	};
	
	/// Constructor. Takes a type of interrupt \p t, which defaults to Type::FINISHED.
	Interrupt(Type t = Type::FINISHED)
		: 	type(t) 
	{ 
		// do nothing else
	}
	
	/// Destructor.
	virtual ~Interrupt() {;}
	
	/// An override from message which will print out details of the message.
	virtual void serialize(std::ostream& os) const override
	{
		os << GetMessageType() << ": type = " << static_cast<std::underlying_type<Type>::type>(type);
	}
	
	/// A static method allowing this class to be used by BundleAccess. Provides the type of message. Note that the type of this message is "pipe::Interrupt" and not the interrupt type chosen in the constructor.
	static const std::string& GetMessageType()
	{
		static std::string MessageType = "pipe::Interrupt";
		return MessageType;
	}
	
	/// The interrupt type.
	Type type;
};
	
} // namespace pipe

#endif
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

/// \class ControlMessage
/// \brief A Message used to control the behavior of modules.
///
/// A ControlMessage conveys some behavioral instructions to modules. A ControlMessage may instruct a module to shut down its execution or trigger a module reset. A ControlMessage should be set by the controller of the pipeline.

struct ControlMessage : public Message
{
	/// The type of the control message
	enum class Type
	{
		NORMAL, ///< Normal operation
		SHUTDOWN, ///< Initiate shutdown operations
		SOFT_RESET ///< Perform an internal reset. This message can be used to indicate milestones in the data or the end of events.
	};
	
	/// Constructor. Takes a control message type \p t, which defaults to Type::NORMAL.
	ControlMessage(Type t = Type::NORMAL)
		: 	type(t) 
	{ 
		// do nothing else
	}
	
	/// Destructor.
	virtual ~ControlMessage() {;}
	
	/// An override from message which will print out details of the message.
	virtual void serialize(std::ostream& os) const override
	{
		os << MessageType << ": type = " << static_cast<std::underlying_type<Type>::type>(type);
	}
	
	/// A static method allowing this class to be used by BundleAccess. Provides the type of message. Note that the type of this message is "pipe::ControlMessage" and not the control message type chosen in the constructor.
	static const std::string& GetMessageType()
	{
		static std::string MessageType = "pipe::ControlMessage";
		return MessageType;
	}
	
	/// The type of ControlMessage
	Type type;
	
};


} // namespace pipe

#endif
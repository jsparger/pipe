//---------------------------------------------------------------------------//
// Copyright (c) 2014 John Sparger <jsparger87@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See https://github.com/jsparger/pipe for more information.
//---------------------------------------------------------------------------//

#ifndef PIPE_MESSAGE_HH
#define PIPE_MESSAGE_HH

#include <ostream>

namespace pipe {

/// \class Message
/// \brief An abstract base class for messages.
///
/// This base class for messages is very minimal and almost a tag. It provides only a standardized way for a message to print be printed to a stream.
class Message
{
public:
	virtual ~Message() {;}
	
	/// Print out the message the stream. Providing an implementation for this method will allow the message to be printed by a class like BundlePrinter.
	virtual void serialize(std::ostream& os) const  = 0;
};

} // namespace pipe

/// an overload for the stream input operator for Messages.
inline std::ostream& operator << (std::ostream& os, const pipe::Message& m)
{
	m.serialize(os);
	return os;
}

#endif
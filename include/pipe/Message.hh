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

class Message
{
public:
	virtual ~Message() {;}
	
	// print out the message.
	virtual void serialize(std::ostream& os) const  = 0;
};

} // namespace pipe

inline std::ostream& operator << (std::ostream& os, const pipe::Message& m)
{
	m.serialize(os);
	return os;
}

#endif
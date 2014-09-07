//---------------------------------------------------------------------------//
// Copyright (c) 2014 John Sparger <jsparger87@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// See https://github.com/jsparger/pipe for more information.
//---------------------------------------------------------------------------//

#include "utkVme/pipe/Message.hh"

std::ostream& operator << (std::ostream& os, const utkVme::pipe::Message& m)
{
	m.serialize(os);
	return os;
}
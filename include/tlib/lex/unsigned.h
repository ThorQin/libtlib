/************************************************************************
*
*  LibTLib
*  Copyright (C) 2010  Thor Qin
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*
* Author: Thor Qin
* Bug Report: thor.qin@gmail.com
*
**************************************************************************/

#ifndef UNSIGNED_H_
#define UNSIGNED_H_

#include "../tlibbase.h"

namespace tlib
{

namespace lex
{

template<class T>
struct make_unsigned { };

template<>
struct make_unsigned<char>
{
  typedef unsigned char type;
};

template<>
struct make_unsigned<signed char>
{
  typedef unsigned char type;
};

template<>
struct make_unsigned<unsigned char>
{
  typedef unsigned char type;
};

template<>
struct make_unsigned<short>
{
  typedef unsigned short type;
};

template<>
struct make_unsigned<unsigned short>
{
  typedef unsigned short type;
};

template<>
struct make_unsigned<int>
{
  typedef unsigned int type;
};

template<>
struct make_unsigned<unsigned int>
{
  typedef unsigned int type;
};

template<>
struct make_unsigned<long>
{
  typedef unsigned long type;
};

template<>
struct make_unsigned<unsigned long>
{
  typedef unsigned long type;
};

} // End of namespace lex
} // End of namespace tlib

#endif /* UNSIGNED_H_ */


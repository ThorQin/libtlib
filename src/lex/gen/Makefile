#########################################################################
#
#  LibTLib
#  Copyright (C) 2010  Thor Qin
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
#
# Author: Thor Qin
# Bug Report: thor.qin@gmail.com
#
#########################################################################

all: clean ../lexbc.cpp

../lexbc.cpp: lexname.lex
	lexgen -o lex.bc lexname.lex
	lexgen -u -o lexu.bc lexname.lex
	lexgen -w -o lexw.bc lexname.lex
	mkres --name lex_name_bc lex.bc > ../lexbc.cpp
	mkres --name lex_name_ubc lexu.bc >> ../lexbc.cpp
	mkres --name lex_name_wbc lexw.bc >> ../lexbc.cpp
	rm -f *.bc

clean:
	-rm -f *.bc
	-rm -f ../lexbc.cpp

.PHONY: all
.PHONY: clean

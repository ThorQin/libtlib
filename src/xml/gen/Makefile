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

all: clean \
../sax_bc.cpp \
../path_bc.cpp \
../predicate_bc.cpp \
../name_check_bc.cpp \
../node_name_bc.cpp \
../node_entry_bc.cpp \
../encoding_bc.cpp

../sax_bc.cpp: sax.lex
	lexgen -o sax.bc sax.lex
	mkres --name sax_bc sax.bc > ../sax_bc.cpp
	rm -f sax.bc
	
../path_bc.cpp: path.lex
	lexgen -w -o path.bc path.lex
	mkres --name path_bc path.bc > ../path_bc.cpp
	rm -f path.bc

../predicate_bc.cpp: predicate.lex
	lexgen -w -o predicate.bc predicate.lex
	mkres --name predicate_bc predicate.bc > ../predicate_bc.cpp
	rm -f predicate.bc
	
../name_check_bc.cpp: name_check.lex
	lexgen -w -o name_check.bc name_check.lex
	mkres --name name_check_bc name_check.bc > ../name_check_bc.cpp
	rm -f name_check.bc
	
../node_name_bc.cpp: node_name.lex
	lexgen -w -o node_name.bc node_name.lex
	mkres --name node_name_bc node_name.bc > ../node_name_bc.cpp
	rm -f node_name.bc
	
../node_entry_bc.cpp: node_entry.lex
	lexgen -o node_entry.bc node_entry.lex
	mkres --name node_entry_bc node_entry.bc > ../node_entry_bc.cpp
	rm -f node_entry.bc
	
../encoding_bc.cpp: encoding.lex
	lexgen -o encoding.bc encoding.lex
	mkres --name encoding_bc encoding.bc > ../encoding_bc.cpp
	rm -f encoding.bc

clean:
	-rm -f *.bc
	-rm -f ../sax_bc.cpp
	-rm -f ../path_bc.cpp
	-rm -f ../predicate_bc.cpp
	-rm -f ../name_check_bc.cpp
	-rm -f ../node_name_bc.cpp
	-rm -f ../node_entry_bc.cpp
	-rm -f ../encoding_bc.cpp

.PHONY: all
.PHONY: clean

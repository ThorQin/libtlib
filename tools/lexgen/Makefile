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

CC = g++
CFLAGS = -DNDEBUG -O2 -Wall
OBJECTS = lexgen.o
INCFLAGS = 
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS = -L../../build -ltlib -lglib-2.0

all: lexgen Makefile

lexgen: $(OBJECTS)
	$(CC) -o lexgen $(OBJECTS) $(LDFLAGS) $(LIBS)

.SUFFIXES:
.SUFFIXES:	.cpp .o

sinclude ${OBJECTS:.o=.d}

%.o: %.cpp
	$(CC) -o $@ -c $(CFLAGS) $< $(INCFLAGS)
	
%.d: %.c
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

count:
	wc *.cpp *.h

clean:
	rm -f *.o; \
	rm -f *.d; \
	rm -f lexgen

install:
	cp lexgen /usr/bin/lexgen; \
	chmod 755 /usr/bin/lexgen

uninstall:
	rm /usr/bin/lexgen

.PHONY: all
.PHONY: count
.PHONY: clean
.PHONY: install
.PHONY: uninstall


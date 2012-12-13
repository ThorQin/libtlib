#############################################################################
# 
#   mkres - Make file to cpp source code.
#   Copyright (C) 2010  Thor Qin
# 
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
# 
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
# 
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
# 
#   Author: Thor Qin
#   Bug Report: thor.qin@gmail.com
# 
#############################################################################

CC = gcc
CFLAGS = -DNDEBUG -O2 -Wall
OBJECTS = mkres.o
INCFLAGS = 
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS =

all: mkres Makefile

mkres: $(OBJECTS)
	$(CC) -o mkres $(OBJECTS) $(LDFLAGS) $(LIBS)

.SUFFIXES:
.SUFFIXES:	.c .o

sinclude ${OBJECTS:.o=.d}

%.o: %.c
	$(CC) -o $@ -c $(CFLAGS) $< $(INCFLAGS)
	
%.d: %.c
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

count:
	wc *.c *.h

clean:
	rm -f *.o; \
	rm -f *.d; \
	rm -f mkres

install:
	cp -f mkres /usr/bin/mkres

uninstall:
	rm -f /usr/bin/mkres

.PHONY: all
.PHONY: count
.PHONY: clean
.PHONY: install
.PHONY: uninstall

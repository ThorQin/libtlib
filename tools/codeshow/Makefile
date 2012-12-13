#############################################################################
# 
#   CodeShow - a simple source code rendering tools.
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


# Depends: tlib-1.0

BIN = $(DESTDIR)/usr/bin

all:
	$(MAKE) -C ./build

clean:
	$(MAKE) -C ./build clean

install:
	install -m755 ./build/codeshow $(BIN)

uninstall:
	rm -f $(BIN)/codeshow

.PHONY: all
.PHONY: clean
.PHONY: install
.PHONY: uninstall

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

#! sp: 	[ \r\n\t]+
#! name: [^ \r\n\t\v\f\[\]\(\)\^\|\+\*\?\\&!#%=<>{}/'";,@~`]+

# Operator
o-start-beg:	<
o-start-end: 	>
o-close-beg:	</
o-close-end:	/>
o-equal:		=
o-slash:		/
 
# PI
pi-xml(m):		<\?xml{sp}.+\?>
pi(m):			<\?{name}([ \r\n\t]+.+)*\?>

# Comment
comment(m):		<!--.*-->

# CData
cdata(m): 		<!\[CDATA\[.*\]\]>

# String
str:			("[^\"]*")|('[^\']*')

# Id
name : 			{name}

# Entry
el: 			&lt;
eg: 			&gt;
ea: 			&amp;
ep: 			&apos;
eq: 			&quot;
ex:				&#x[a-fA-F0-9]+;
ed:				&#[0-9]+;
e:				&[a-zA-Z0-9]+;

# Space
sp:				{sp}

# Normal text
text:			[^ \r\n\t\v\f\&\=\<\>\/]+

# Any others as an error


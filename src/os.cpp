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

#include "../include/tlib/os.h"
#ifdef __MSVC__
#  ifndef _WINSOCKAPI_
#    define _WINSOCKAPI_
#  endif
#  define _WIN32_WINNT 0x0500
#  define WINVER 0x0500
#  include <windows.h>
#  include <direct.h>
#elif defined(__GNUC__)
#  include <unistd.h>
#  if defined(__MINGW32__) || defined(__CYGWIN__)
#    define _WIN32_WINNT 0x0500
#    define WINVER 0x0500
#    include <windows.h>
#    include <direct.h>
#    include <limits.h>
#  else
#    include <linux/limits.h>
#  endif
#endif

namespace tlib {

std::string get_host_name() {
	char hostname[512] = { 0 };
#if defined(__MSVC__)
	DWORD host_size = 512;
	if (!GetComputerNameExA(ComputerNameDnsFullyQualified, hostname, &host_size))
	strcpy_s(hostname, 512, "computer.local");
#elif defined(__MINGW32__)
	DWORD host_size = 512;
	if (!GetComputerNameExA(ComputerNameDnsFullyQualified, hostname, &host_size))
	strcpy(hostname, "computer.local");
#elif defined(__GNUC__)
	::gethostname(hostname, 512);
#else
#error Not implements.
#endif
	return hostname;
}

std::string get_current_path() {
#if defined(__MSVC__)
	char buf[MAX_PATH];
	if (_getcwd(buf, MAX_PATH) != 0)
	return buf;
	else
	return "";
#elif defined(__GNUC__)
	char buf[PATH_MAX];
	if (getcwd(buf, PATH_MAX) != 0)
		return buf;
	else
		return "";
#endif
}

std::string get_exec_filename() {
#if defined(__MSVC__) || defined(__MINGW32__)
	char buf[MAX_PATH];
	GetModuleFileNameA(0, buf, MAX_PATH);
	return buf;
#elif defined(__GNUC__)
	char buf[PATH_MAX];
	int rslt = readlink("/proc/self/exe", buf, PATH_MAX);
	if (rslt < 0 || rslt >= PATH_MAX)
		return "";
	else {
		buf[rslt] = 0;
		return buf;
	}
#endif
}

// Full path with no filename.
std::string get_exec_path() {
	std::string filename = get_exec_filename();
	if (filename.empty())
		return "";
#ifdef __MSVC__
	replace<char>(filename, "/", "\\");
	std::string::size_type pos = filename.rfind('\\');
#elif defined(__GNUC__)
#  if defined(__MINGW32__)
	std::string::size_type pos = filename.rfind('\\');
#  else
	std::string::size_type pos = filename.rfind('/');
#  endif
#endif
	if (pos != std::string::npos)
		return filename.substr((std::string::size_type) 0, pos);
	else
		return "";
}

// Full path of current logged-in user's home path.
std::string get_home_path() {
#if defined(__GNUC__)
#if defined(__MINGW32__)
	const char* home = getenv("USERPROFILE");
#else
	const char* home = getenv("HOME");
#endif
#elif defined(__MSVC__)
	const char* home = getenv("USERPROFILE");
#else
#error "not implement..."
#endif
	if (home)
		return home;
	else
		return "";
}

}

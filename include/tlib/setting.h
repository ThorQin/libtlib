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


#ifndef TSETTING_H_
#define TSETTING_H_

#include "binary.h"
#include "charset.h"
#include "strfunc.h"
#include <map>

namespace tlib {

template<typename TChar>
class Setting {
private:
	template<typename TFilenameChar>
	inline Setting(const std::basic_string<TFilenameChar>& filename);

	Setting& operator =(const Setting&);
public:
	typedef const std::basic_string<TChar> StrType;
	template<typename TFilenameChar>
	static inline Setting load(const std::basic_string<TFilenameChar>& filename)
			throw (std::runtime_error);

	template<typename TFilenameChar>
	static inline Setting create(
			const std::basic_string<TFilenameChar>& filename)
					throw (std::runtime_error);

	void set(const StrType& key, const StrType& value);
	StrType get(const StrType& key,
			const StrType& defaultValue = StrType());

	void clear();
	void remoev(const std::basic_string<TChar>& key);
	void save();

private:
	typedef std::map<StrType, StrType> Dict;
	std::string _filename;
	Dict _dict;
};


template <typename T_Char>
const T_Char* _new_line();
template <> inline
const char* _new_line<char>()
{
	return "\n";
}
template <> inline
const wchar_t* _new_line<wchar_t>()
{
	return L"\n";
}


template<typename TChar>
template<typename TFilenameChar>
inline Setting<TChar>::Setting(
		const std::basic_string<TFilenameChar>& filename) {
	_filename = str<char>(filename);
}

template<typename TChar>
template<typename TFilenameChar>
inline Setting<TChar> Setting<TChar>::load(
		const std::basic_string<TFilenameChar>& filename)
				throw (std::runtime_error) {
	Setting<TChar> setting(filename);
	std::basic_string<TChar> instr = load_string<TChar>(setting._filename);
	std::vector<std::basic_string<TChar> > lines = split<TChar>(instr,
			_new_line<TChar>());
	typedef typename std::vector<std::basic_string<TChar> >::iterator iterator;
	for (iterator line = lines.begin(); line != lines.end(); line++) {
		trim(*line);
		if ((*line).empty())
			continue;
		if ((*line)[0] == (TChar) '#')
			continue;
		size_t fnd = (*line).find((TChar) '=');
		if (fnd != std::string::npos) {
			std::basic_string<TChar> key = (*line).substr(0, fnd);
			tlib::trim_right(key);
			if (!key.empty())
				setting.set(key, tlib::trim_left_copy((*line).substr(fnd + 1)));
				//_dict[key] = tlib::trim_left_copy((*line).substr(fnd + 1));
		} else {
			//_dict[*line] = *line;
			setting.set(*line, *line);
		}
	}
	return setting;
}

template<typename TChar>
template<typename TFilenameChar>
inline Setting<TChar> Setting<TChar>::create(
		const std::basic_string<TFilenameChar>& filename)
				throw (std::runtime_error) {
	return Setting<TChar>(filename);
}

template<typename TChar>
inline void Setting<TChar>::clear() {
	_dict.clear();
}

template<typename TChar>
inline void Setting<TChar>::remoev(const std::basic_string<TChar>& key) {
	_dict.remove(key);
}

template <typename T_Char>
const std::string _to_utf8(const std::basic_string<T_Char>& str);
template <> inline
const std::string _to_utf8<char>(const std::string& str)
{
	return tlib::locale_to_utf8(str);
}
template <> inline
const std::string _to_utf8<wchar_t>(const std::wstring& str)
{
	return tlib::wstring_to_utf8(str);
}

template<typename TChar>
inline void Setting<TChar>::save() {
	std::ofstream outfile;
	outfile.open(_filename.c_str(), std::ios::out | std::ios::trunc);
	if (!outfile.good()) {
		throw std::runtime_error("Open file failed.");
	}
	outfile << "\xEF\xBB\xBF";
	typedef typename Dict::const_iterator const_iterator;
	for (const_iterator it = _dict.begin(); it != _dict.end(); it++)
	{
		outfile << _to_utf8<TChar>(it->first) << "="
				<< _to_utf8<TChar>(it->second) << "\n";
	}
	outfile.close();
}

template<typename TChar>
void Setting<TChar>::set(const StrType& key, const StrType& value) {
	StrType finalKey = replace_copy(key, str<TChar>("="), str<TChar>("."));
	_dict[finalKey] = value;
}

template<typename TChar>
typename Setting<TChar>::StrType Setting<TChar>::get(
		const StrType& key,
		const StrType& defaultValue) {
	typename Dict::iterator it = _dict.find(key);
	if (it == _dict.end()) {
		return defaultValue;
	} else {
		return it.second;
	}
}

} // End of namespace tlib

#endif /* TSETTING_H_ */

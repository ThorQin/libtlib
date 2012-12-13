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

#ifndef TLIBTIME_H_
#define TLIBTIME_H_

#include "strfunc.h"
#include "charset.h"
#include <time.h>

namespace tlib {

enum class SpanUnit : int {
	seconds, minutes, hours, days, weeks, years
};

class TimeSpan {
public:
	inline explicit TimeSpan(time_t seconds,
			SpanUnit displayUnit = SpanUnit::seconds);
	inline TimeSpan(const TimeSpan& other);

	inline void set_display_unit(SpanUnit displayUnit);
	inline SpanUnit get_display_unit() const;
	inline void set_total_seconds(time_t span);
	inline time_t get_total_seconds() const;
	time_t get_seconds() const;
	time_t get_minutes() const;
	time_t get_hours() const;
	time_t get_days() const;
	time_t get_weeks() const;
	time_t get_years() const;
	inline bool is_forward() const;
	inline TimeSpan& operator =(const TimeSpan& other);
	inline TimeSpan& operator +=(const TimeSpan& other);
	inline TimeSpan& operator -=(const TimeSpan& other);
private:
	time_t _span;
	SpanUnit _unit;
};

inline TimeSpan::TimeSpan(time_t seconds, SpanUnit displayUnit) :
		_span(seconds), _unit(displayUnit) {
}
inline TimeSpan::TimeSpan(const TimeSpan& other) :
		_span(other._span), _unit(other._unit) {
}
inline void TimeSpan::set_display_unit(SpanUnit displayUnit) {
	_unit = displayUnit;
}
inline SpanUnit TimeSpan::get_display_unit() const {
	return _unit;
}
inline void TimeSpan::set_total_seconds(time_t span) {
	_span = span;
}
inline time_t TimeSpan::get_total_seconds() const {
	return _span;
}
inline bool TimeSpan::is_forward() const {
	return (_span >= 0);
}
inline TimeSpan& TimeSpan::operator =(const TimeSpan& other) {
	_span = other._span;
	return *this;
}
inline TimeSpan& TimeSpan::operator +=(const TimeSpan& other) {
	_span += other._span;
	return *this;
}
inline TimeSpan& TimeSpan::operator -=(const TimeSpan& other) {
	_span -= other._span;
	return *this;
}
inline const tlib::TimeSpan operator +(const tlib::TimeSpan& span1,
		const tlib::TimeSpan& span2) {
	tlib::TimeSpan result(span1);
	result += span2;
	return result;
}
inline const tlib::TimeSpan operator -(const tlib::TimeSpan& span1,
		const tlib::TimeSpan& span2) {
	tlib::TimeSpan result(span1);
	result -= span2;
	return result;
}






enum class TimeFormat : int {
	format_standard,
	format_date,
	format_time,
	format_utc_standard,
	format_utc_date,
	format_utc_time,
	format_mail
};

class Time {
public:
	inline Time();
	explicit inline Time(time_t tm);
	inline Time(const Time& other);
	explicit inline Time(const std::string& stime);

	const std::string to_string(TimeFormat format =
			TimeFormat::format_standard) const;
	void parse(const std::string& stime, TimeFormat format =
			TimeFormat::format_standard);
	inline void parse(const std::wstring& stime, TimeFormat format =
			TimeFormat::format_standard);
	void get_utc_time(struct tm& tm) const;
	void set_utc_time(struct tm& tm);
	void get_local_time(struct tm& tm) const;
	void set_local_time(struct tm& tm);
	void update();

	inline Time& operator =(const Time& other);
	inline Time& operator +=(const TimeSpan& span);
	inline Time& operator -=(const TimeSpan& span);
	time_t _tm;
};


inline Time::Time() {
	time(&_tm);
}
inline Time::Time(time_t tm) :
		_tm(tm) {
}
inline Time::Time(const Time& other) :
		_tm(other._tm) {
}
inline Time::Time(const std::string& stime) {
	time(&_tm);
	parse(stime, TimeFormat::format_standard);
}
inline Time& Time::operator =(const Time& other) {
	_tm = other._tm;
	return *this;
}
inline Time& Time::operator +=(const TimeSpan& span) {
	_tm += span.get_total_seconds();
	return *this;
}
inline Time& Time::operator -=(const TimeSpan& span) {
	_tm -= span.get_total_seconds();
	return *this;
}
inline Time parse_time(const std::string& stime,
		TimeFormat format = TimeFormat::format_standard) {
	Time t;
	t.parse(stime, format);
	return t;
}
inline Time parse_time(const std::wstring& stime,
		TimeFormat format = TimeFormat::format_standard) {
	Time t;
	t.parse(stime, format);
	return t;
}


inline void Time::parse(const std::wstring& stime, TimeFormat format) {
	this->parse(wstring_to_locale(stime), format);
}

inline int get_day(const struct tm& tm) {
	return tm.tm_mday;
}
inline int get_month(const struct tm& tm) {
	return tm.tm_mon + 1;
}
inline int get_year(const struct tm& tm) {
	return tm.tm_year + 1900;
}
// '0' is sun day.
inline int get_week(const struct tm& tm) {
	return tm.tm_wday;
}
inline int get_hour(const struct tm& tm) {
	return tm.tm_hour;
}
inline int get_minute(const struct tm& tm) {
	return tm.tm_min;
}
inline int get_second(const struct tm& tm) {
	return tm.tm_sec;
}
inline void set_day(struct tm& tm, int day) {
	tm.tm_mday = day;
}
inline void set_month(struct tm& tm, int month) {
	tm.tm_mon = month - 1;
}
inline void set_year(struct tm& tm, int year) {
	tm.tm_year = year - 1900;
}
inline void set_hour(struct tm& tm, int hour) {
	tm.tm_hour = hour;
}
inline void set_minute(struct tm& tm, int minute) {
	tm.tm_min = minute;
}
inline void set_second(struct tm& tm, int second) {
	tm.tm_sec = second;
}

}

const tlib::Time operator +(const tlib::Time& tm, const tlib::TimeSpan& span);
const tlib::Time operator -(const tlib::Time& tm, const tlib::TimeSpan& span);
const tlib::TimeSpan operator -(const tlib::Time& tm1, const tlib::Time& tm2);
const tlib::TimeSpan operator +(const tlib::TimeSpan& span1,
		const tlib::TimeSpan& span2);
const tlib::TimeSpan operator -(const tlib::TimeSpan& span1,
		const tlib::TimeSpan& span2);

#endif /* TLIBTIME_H_ */

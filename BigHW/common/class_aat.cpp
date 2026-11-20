/* BigHW */

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <climits>
#include "../include/class_aat.h"
using namespace std;

#if !ENABLE_LIB_COMMON_TOOLS //不使用lib才有效

/* ---------------------------------------------------------------
	 允许加入其它需要static函数（内部工具用）
   ---------------------------------------------------------------- */

/***************************************************************************
  函数名称：aat_is_option
  功    能：判断一个命令行参数是不是 "--" 开头的有效可变参数名称
  输入参数：str：待判断的字符串
  返 回 值：true - 是可变参数；false - 不是（固定参数、"-x"、"--"、空串等）
  说    明：
 ***************************************************************************/
static bool aat_is_option(const char* const str)
{
	if (str == NULL) {
		return false;
	}
	if (str[0] != '-' || str[1] != '-') {
		return false;
	}
	return (str[2] != '\0');
}

/***************************************************************************
  函数名称：aat_str_to_int
  功    能：把字符串解析成 int，用于 int 型附加参数
  输入参数：str   ：待解析的字符串（可带一个 '+' / '-' 号，其余必须全是数字）
  返 回 值：0 - 成功，value 带回结果；-1 - 不是合法的整数写法
  说    明：
 ***************************************************************************/
static int aat_str_to_int(const char* const str, int& value)
{
	if (str == NULL || str[0] == '\0') {
		return -1;
	}
	const char* p = str;
	int sign = 1;
	if (*p == '+' || *p == '-') {
		if (*p == '-') {
			sign = -1;
		}
		p++;
	}
	if (*p == '\0') {
		return -1;
	}
	for (const char* q = p; *q != '\0'; q++) {
		if (*q < '0' || *q > '9') {
			return -1;
		}
	}
	long long temp = 0;
	for (const char* q = p; *q != '\0'; q++) {
		temp = temp * 10 + (*q - '0');
		if (temp > 2147483648LL) {
			temp = 2147483648LL;
			break;
		}
	}
	temp *= sign;
	if (temp > INT_MAX) {
		temp = INT_MAX;
	}
	if (temp < INT_MIN) {
		temp = INT_MIN;
	}
	value = (int)temp;
	return 0;
}

/***************************************************************************
  函数名称：aat_str_to_double
  功    能：把字符串解析成 double，用于 double 型附加参数
  输入参数：str   ：待解析的字符串（可带一个 '+' / '-' 号，最多一个小数点，其余全是数字）
  返 回 值：0 - 成功，value 带回结果；-1 - 不是合法的浮点数写法
  说    明：
 ***************************************************************************/
static int aat_str_to_double(const char* const str, double& value)
{
	if (str == NULL) {
		return -1;
	}
	const char* p = str;
	if (*p == '+' || *p == '-') {
		p++;
	}
	int digit_count = 0;
	int dot_count = 0;
	for (; *p != '\0'; p++) {
		if (*p >= '0' && *p <= '9') {
			digit_count++;
		}
		else if (*p == '.') {
			if (++dot_count > 1) {
				return -1;
			}
		}
		else {
			return -1;
		}
	}
	if (digit_count == 0) {
		return -1;
	}
	value = atof(str);
	return 0;
}

/***************************************************************************
  函数名称：aat_str_to_ipaddr
  功    能：把 "127.0.0.1" 形式的字符串解析成 u_int32 型的 0x7f000001
  输入参数：str   ：待解析的字符串
  返 回 值：true - 成功，value 带回结果；false - 不是合法的点分十进制 IP 地址
  说    明：
 ***************************************************************************/
static bool aat_str_to_ipaddr(const char* const str, u_int& value)
{
	if (str == NULL) {
		return false;
	}
	const char* p = str;
	u_int ip = 0;
	for (int i = 0; i < 4; i++) {
		if (*p < '0' || *p > '9') {
			return false;
		}
		int part = 0;
		while (*p >= '0' && *p <= '9') {
			part = part * 10 + (*p - '0');
			if (part > 255) {
				return false;
			}
			p++;
		}
		ip = (ip << 8) | (u_int)part;
		if (i < 3) {
			if (*p != '.') {
				return false;
			}
			p++;
		}
	}
	value = ip;
	return (*p == '\0');
}

/***************************************************************************
  函数名称：aat_ipaddr_to_string
  功    能：把 u_int32 型的 IP 地址还原成 "127.0.0.1" 形式
  输入参数：ipaddr：IP地址的 u_int32 形式
  返 回 值：点分十进制字符串
  说    明：
 ***************************************************************************/
static string aat_ipaddr_to_string(const u_int ipaddr)
{
	ostringstream oss;
	oss << ((ipaddr >> 24) & 0xFFu) << '.' << ((ipaddr >> 16) & 0xFFu) << '.'
		<< ((ipaddr >> 8) & 0xFFu) << '.' << (ipaddr & 0xFFu);
	return oss.str();
}

/***************************************************************************
  函数名称：aat_int_to_string
  功    能：int 转字符串（十进制）
  输入参数：value：待转换的整数
  返 回 值：十进制字符串
  说    明：
 ***************************************************************************/
static string aat_int_to_string(const int value)
{
	ostringstream oss;
	oss << value;
	return oss.str();
}

/***************************************************************************
  函数名称：aat_double_to_string
  功    能：double 转字符串（固定 6 位小数）
  输入参数：value：待转换的浮点数
  返 回 值："1.230000" 这种形式
  说    明：
 ***************************************************************************/
static string aat_double_to_string(const double value)
{
	ostringstream oss;
	oss << fixed << setprecision(6) << value;
	return oss.str();
}

/***************************************************************************
  函数名称：aat_general_to_string
  功    能：double 转字符串（默认 6 位有效数字，即 %g 的效果）
  输入参数：value：待转换的浮点数
  返 回 值："1.23"、"99.9" 这种形式
  说    明：
 ***************************************************************************/
static string aat_general_to_string(const double value)
{
	ostringstream oss;
	oss << value;
	return oss.str();
}

template <typename T>
struct aat_set_traits {
	static bool equal(const T& lhs, const T& rhs) {
		return lhs == rhs;
	}
};

template <>
struct aat_set_traits<double> {
	static bool equal(const double& lhs, const double& rhs) {
		return (lhs > rhs - DOUBLE_DELTA) && (lhs < rhs + DOUBLE_DELTA);
	}
};

/***************************************************************************
  函数名称：aat_set_count
  功    能：统计以 sentinel 结尾的集合元素个数
  输入参数：set     ：集合首地址，以 sentinel 结尾
			sentinel：集合结束标志（int 用 INVALID_INT_VALUE_OF_SET，
			          double 用 INVALID_DOUBLE_VALUE_OF_SET，string 用空串）
  返 回 值：元素个数（set 为 NULL 时返回 0）
  说    明：
 ***************************************************************************/
template <typename T>
static int aat_set_count(const T* const set, const T& sentinel)
{
	int count = 0;
	if (set == NULL) {
		return 0;
	}
	while (!(set[count] == sentinel)) {
		count++;
	}
	return count;
}

/***************************************************************************
  函数名称：aat_in_set
  功    能：判断一个值是否属于集合
  输入参数：set     ：集合首地址，以 sentinel 结尾
			sentinel：集合结束标志
			value   ：待判断的值
  返 回 值：true - 在集合中；false - 不在
  说    明：
 ***************************************************************************/
template <typename T>
static bool aat_in_set(const T* const set, const T& sentinel, const T& value)
{
	const int count = aat_set_count(set, sentinel);
	for (int i = 0; i < count; i++) {
		if (aat_set_traits<T>::equal(set[i], value)) {
			return true;
		}
	}
	return false;
}

/***************************************************************************
  函数名称：aat_int_set_to_string
  功    能：把 int 型集合拼成 "11/22/33" 形式
  输入参数：set：集合首地址
  返 回 值：拼接后的字符串
  说    明：
 ***************************************************************************/
static string aat_int_set_to_string(const int* const set)
{
	const int count = aat_set_count(set, INVALID_INT_VALUE_OF_SET);
	string result;
	for (int i = 0; i < count; i++) {
		if (i > 0) {
			result += '/';
		}
		result += aat_int_to_string(set[i]);
	}
	return result;
}

/***************************************************************************
  函数名称：aat_double_set_to_string
  功    能：把 double 型集合拼成 "1.100000/2.200000" 形式
  输入参数：set：集合首地址
  返 回 值：拼接后的字符串
  说    明：
 ***************************************************************************/
static string aat_double_set_to_string(const double* const set)
{
	const int count = aat_set_count(set, INVALID_DOUBLE_VALUE_OF_SET);
	string result;
	for (int i = 0; i < count; i++) {
		if (i > 0) {
			result += '/';
		}
		result += aat_double_to_string(set[i]);
	}
	return result;
}

/***************************************************************************
  函数名称：aat_double_set_to_general_string
  功    能：把 double 型集合拼成 "1.1/2.2/3.3" 形式（%g 风格）
  输入参数：set：集合首地址
  返 回 值：拼接后的字符串
  说    明：
 ***************************************************************************/
static string aat_double_set_to_general_string(const double* const set)
{
	const int count = aat_set_count(set, INVALID_DOUBLE_VALUE_OF_SET);
	string result;
	for (int i = 0; i < count; i++) {
		if (i > 0) {
			result += '/';
		}
		result += aat_general_to_string(set[i]);
	}
	return result;
}

/***************************************************************************
  函数名称：aat_string_set_to_string
  功    能：把 string 型集合拼成 "md5/sha1/..." 形式
  输入参数：set：集合首地址
  返 回 值：拼接后的字符串
  说    明：
 ***************************************************************************/
static string aat_string_set_to_string(const string* const set)
{
	const int count = aat_set_count(set, string(""));
	string result;
	for (int i = 0; i < count; i++) {
		if (i > 0) {
			result += '/';
		}
		result += set[i];
	}
	return result;
}

/***************************************************************************
  函数名称：args_analyse_tools::args_analyse_tools
  功    能：
  输入参数：
  返 回 值：
  说    明：null
 ***************************************************************************/
args_analyse_tools::args_analyse_tools()
{
	this->args_name = "";
	this->extargs_type = ST_EXTARGS_TYPE::null;
	this->extargs_num = 0;

	this->extargs_bool_default = false;

	this->extargs_int_default = 0;
	this->extargs_int_min = 0;
	this->extargs_int_max = 0;
	this->extargs_int_set = NULL;

	this->extargs_double_default = 0.0;
	this->extargs_double_min = 0.0;
	this->extargs_double_max = 0.0;
	this->extargs_double_set = NULL;

	this->extargs_string_default = "";
	this->extargs_string_set = NULL;

	this->extargs_ipaddr_default = 0;

	this->args_existed = 0;
	this->extargs_int_value = 0;
	this->extargs_double_value = 0.0;
	this->extargs_string_value = "";
	this->extargs_ipaddr_value = 0;
}

/***************************************************************************
  函数名称：args_analyse_tools::args_analyse_tools
  功    能：
  输入参数：
  返 回 值：
  说    明：boolean
 ***************************************************************************/
args_analyse_tools::args_analyse_tools(const char* name, const ST_EXTARGS_TYPE type, const int ext_num, const bool def)
	: args_analyse_tools()
{
	this->args_name = (name != NULL) ? string(name) : string("");
	this->extargs_type = type;
	this->extargs_num = ext_num;
	this->extargs_bool_default = def;
}

/***************************************************************************
  函数名称：args_analyse_tools::args_analyse_tools
  功    能：
  输入参数：
  返 回 值：
  说    明：int_with_default、int_with_error
 ***************************************************************************/
args_analyse_tools::args_analyse_tools(const char* name, const ST_EXTARGS_TYPE type, const int ext_num, const int def, const int _min, const int _max)
	: args_analyse_tools()
{
	this->args_name = (name != NULL) ? string(name) : string("");
	this->extargs_type = type;
	this->extargs_num = ext_num;
	this->extargs_int_default = def;
	this->extargs_int_min = _min;
	this->extargs_int_max = _max;
	this->extargs_int_value = def;
}

/***************************************************************************
  函数名称：args_analyse_tools::args_analyse_tools
  功    能：
  输入参数：
  返 回 值：
  说    明：int_with_set_default、int_with_set_error
 ***************************************************************************/
args_analyse_tools::args_analyse_tools(const char* name, const enum ST_EXTARGS_TYPE type, const int ext_num, const int def_of_set_pos, const int* const set)
	: args_analyse_tools()
{
	this->args_name = (name != NULL) ? string(name) : string("");
	this->extargs_type = type;
	this->extargs_num = ext_num;
	this->extargs_int_set = const_cast<int*>(set);
	int default_value = 0;
	const int count = aat_set_count(set, INVALID_INT_VALUE_OF_SET);
	if (def_of_set_pos >= 0 && def_of_set_pos < count) {
		default_value = set[def_of_set_pos];
	}
	this->extargs_int_default = default_value;
	this->extargs_int_value = default_value;
}

/***************************************************************************
  函数名称：args_analyse_tools::args_analyse_tools
  功    能：
  输入参数：
  返 回 值：
  说    明：str、ipaddr_with_default、ipaddr_with_error
 ***************************************************************************/
args_analyse_tools::args_analyse_tools(const char* name, const ST_EXTARGS_TYPE type, const int ext_num, const string def)
	: args_analyse_tools()
{
	this->args_name = (name != NULL) ? string(name) : string("");
	this->extargs_type = type;
	this->extargs_num = ext_num;
	this->extargs_string_default = def;
	this->extargs_string_value = def;
	if (type == ST_EXTARGS_TYPE::ipaddr_with_default || type == ST_EXTARGS_TYPE::ipaddr_with_error) {
		u_int ipaddr = 0;
		if (!aat_str_to_ipaddr(def.c_str(), ipaddr)) {
			ipaddr = 0;
		}
		this->extargs_ipaddr_default = ipaddr;
		this->extargs_ipaddr_value = ipaddr;
	}
}

/***************************************************************************
  函数名称：args_analyse_tools::args_analyse_tools
  功    能：
  输入参数：
  返 回 值：
  说    明：str_with_set_default、str_with_set_error
 ***************************************************************************/
args_analyse_tools::args_analyse_tools(const char* name, const ST_EXTARGS_TYPE type, const int ext_num, const int def_of_set_pos, const string* const set)
	: args_analyse_tools()
{
	this->args_name = (name != NULL) ? string(name) : string("");
	this->extargs_type = type;
	this->extargs_num = ext_num;
	this->extargs_string_set = const_cast<string*>(set);
	string default_value = "";
	const int count = aat_set_count(set, string(""));
	if (def_of_set_pos >= 0 && def_of_set_pos < count) {
		default_value = set[def_of_set_pos];
	}
	this->extargs_string_default = default_value;
	this->extargs_string_value = default_value;
}

/***************************************************************************
  函数名称：args_analyse_tools::args_analyse_tools
  功    能：
  输入参数：
  返 回 值：
  说    明：double_with_default、double_with_error
 ***************************************************************************/
args_analyse_tools::args_analyse_tools(const char* name, const ST_EXTARGS_TYPE type, const int ext_num, const double	def, const double _min, const double _max)
	: args_analyse_tools()
{
	this->args_name = (name != NULL) ? string(name) : string("");
	this->extargs_type = type;
	this->extargs_num = ext_num;
	this->extargs_double_default = def;
	this->extargs_double_min = _min;
	this->extargs_double_max = _max;
	this->extargs_double_value = def;
}

/***************************************************************************
  函数名称：args_analyse_tools::args_analyse_tools
  功    能：
  输入参数：
  返 回 值：
  说    明：double_with_set_default、double_with_set_error
 ***************************************************************************/
args_analyse_tools::args_analyse_tools(const char* name, const enum ST_EXTARGS_TYPE type, const int ext_num, const int def_of_set_pos, const double* const set)
	: args_analyse_tools()
{
	this->args_name = (name != NULL) ? string(name) : string("");
	this->extargs_type = type;
	this->extargs_num = ext_num;
	this->extargs_double_set = const_cast<double*>(set);
	double default_value = 0.0;
	const int count = aat_set_count(set, INVALID_DOUBLE_VALUE_OF_SET);
	if (def_of_set_pos >= 0 && def_of_set_pos < count) {
		default_value = set[def_of_set_pos];
	}
	this->extargs_double_default = default_value;
	this->extargs_double_value = default_value;
}

/***************************************************************************
  函数名称：args_analyse_tools::~args_analyse_tools
  功    能：
  输入参数：
  返 回 值：
  说    明：
 ***************************************************************************/
args_analyse_tools::~args_analyse_tools()
{
}

/* ---------------------------------------------------------------
	 允许AAT中自定义成员函数的实现（private）
   ---------------------------------------------------------------- */

/***************************************************************************
  函数名称：args_analyse_tools::get_name
  功    能：
  输入参数：
  返 回 值：
  说    明：已实现，不要动
 ***************************************************************************/
const string args_analyse_tools::get_name() const
{
	return this->args_name;
}

/***************************************************************************
  函数名称：args_analyse_tools::existed
  功    能：
  输入参数：
  返 回 值：
  说    明：加!!后，只能是0/1
 ***************************************************************************/
const int args_analyse_tools::existed() const
{
	return !!this->args_existed;
}

/***************************************************************************
  函数名称：args_analyse_tools::get_int
  功    能：
  输入参数：
  返 回 值：
  说    明：已实现，不要动
 ***************************************************************************/
const int args_analyse_tools::get_int() const
{
	return this->extargs_int_value;
}

/***************************************************************************
  函数名称：args_analyse_tools::get_double
  功    能：
  输入参数：
  返 回 值：
  说    明：已实现，不要动
 ***************************************************************************/
const double args_analyse_tools::get_double() const
{
	return this->extargs_double_value;
}

/***************************************************************************
  函数名称：args_analyse_tools::get_string
  功    能：
  输入参数：
  返 回 值：
  说    明：已实现，不要动
 ***************************************************************************/
const string args_analyse_tools::get_string() const
{
	return this->extargs_string_value;
}

/***************************************************************************
  函数名称：args_analyse_tools::get_ipaddr
  功    能：
  输入参数：
  返 回 值：
  说    明：已实现，不要动
 ***************************************************************************/
const unsigned int args_analyse_tools::get_ipaddr() const
{
	return this->extargs_ipaddr_value;
}

/***************************************************************************
  函数名称：args_analyse_tools::get_str_ipaddr
  功    能：
  输入参数：
  返 回 值：
  说    明：将 extargs_ipaddr_value 的值从 0x7f000001 转为 "127.0.0.1"
 ***************************************************************************/
const string args_analyse_tools::get_str_ipaddr() const
{
	return aat_ipaddr_to_string(this->extargs_ipaddr_value);
}

/***************************************************************************
  函数名称：args_analyse_tools::aat_get_type
  功    能：返回本参数类型对应的名称，用于打印参数表
  输入参数：无
  返 回 值：类型名称字符串
  说    明：
 ***************************************************************************/
const char* args_analyse_tools::aat_get_type() const
{
	switch (this->extargs_type) {
	case ST_EXTARGS_TYPE::boolean:					return "Bool";
	case ST_EXTARGS_TYPE::int_with_default:			return "IntWithDefault";
	case ST_EXTARGS_TYPE::int_with_error:			return "IntWithError";
	case ST_EXTARGS_TYPE::int_with_set_default:		return "IntSETWithDefault";
	case ST_EXTARGS_TYPE::int_with_set_error:		return "IntSETWithError";
	case ST_EXTARGS_TYPE::double_with_default:		return "DoubleWithDefault";
	case ST_EXTARGS_TYPE::double_with_error:		return "DoubleWithError";
	case ST_EXTARGS_TYPE::double_with_set_default:	return "DoubleSETWithDefault";
	case ST_EXTARGS_TYPE::double_with_set_error:	return "DoubleSETWithError";
	case ST_EXTARGS_TYPE::str:						return "String";
	case ST_EXTARGS_TYPE::str_with_set_default:		return "StringSETWithDefault";
	case ST_EXTARGS_TYPE::str_with_set_error:		return "StringSETWithError";
	case ST_EXTARGS_TYPE::ipaddr_with_default:		return "IPAddrWithDefault";
	case ST_EXTARGS_TYPE::ipaddr_with_error:		return "IPAddrWithError";
	default:										return "Undefined";
	}
}

/***************************************************************************
  函数名称：args_analyse_tools::aat_has_default
  功    能：判断本参数是否有缺省值（决定报错信息里要不要打印"缺省:..."）
  输入参数：无
  返 回 值：1 - 有缺省值；0 - 没有
  说    明：
 ***************************************************************************/
const int args_analyse_tools::aat_has_default() const
{
	switch (this->extargs_type) {
	case ST_EXTARGS_TYPE::int_with_default:
	case ST_EXTARGS_TYPE::int_with_set_default:
	case ST_EXTARGS_TYPE::double_with_default:
	case ST_EXTARGS_TYPE::double_with_set_default:
		return 1;
	case ST_EXTARGS_TYPE::str:
	case ST_EXTARGS_TYPE::str_with_set_default:
	case ST_EXTARGS_TYPE::ipaddr_with_default:
		return !this->extargs_string_default.empty();
	default:
		return 0;
	}
}

/***************************************************************************
  函数名称：args_analyse_tools::aat_get_val
  功    能：返回本参数当前值的显示文本，用于打印参数表的"值"列
  输入参数：无
  返 回 值：参数出现时为实际值，未出现时为 "/"
  说    明：
 ***************************************************************************/
const string args_analyse_tools::aat_get_val() const
{
	if (!this->existed()) {
		return "/";
	}
	switch (this->extargs_type) {
		case ST_EXTARGS_TYPE::boolean:
			return "true";
		case ST_EXTARGS_TYPE::int_with_default:
		case ST_EXTARGS_TYPE::int_with_error:
		case ST_EXTARGS_TYPE::int_with_set_default:
		case ST_EXTARGS_TYPE::int_with_set_error:
			return aat_int_to_string(this->extargs_int_value);
		case ST_EXTARGS_TYPE::double_with_default:
		case ST_EXTARGS_TYPE::double_with_error:
		case ST_EXTARGS_TYPE::double_with_set_default:
		case ST_EXTARGS_TYPE::double_with_set_error:
			return aat_double_to_string(this->extargs_double_value);
		case ST_EXTARGS_TYPE::ipaddr_with_default:
		case ST_EXTARGS_TYPE::ipaddr_with_error:
			return aat_ipaddr_to_string(this->extargs_ipaddr_value);
		default:
			return this->extargs_string_value;
	}
}

/***************************************************************************
  函数名称：args_analyse_tools::aat_get_default
  功    能：返回缺省值的显示文本，用于打印参数表的"缺省"列
  输入参数：无
  返 回 值：有缺省值时返回缺省值，否则返回 "/"
  说    明：
 ***************************************************************************/
const string args_analyse_tools::aat_get_default() const
{
	switch (this->extargs_type) {
		case ST_EXTARGS_TYPE::boolean:
			return this->extargs_bool_default ? "true" : "false";
		case ST_EXTARGS_TYPE::int_with_default:
		case ST_EXTARGS_TYPE::int_with_set_default:
			return aat_int_to_string(this->extargs_int_default);
		case ST_EXTARGS_TYPE::double_with_default:
		case ST_EXTARGS_TYPE::double_with_set_default:
			return aat_double_to_string(this->extargs_double_default);
		case ST_EXTARGS_TYPE::str:
		case ST_EXTARGS_TYPE::str_with_set_default:
			return this->extargs_string_default.empty() ? string("/") : this->extargs_string_default;
		case ST_EXTARGS_TYPE::ipaddr_with_default:
			if (this->extargs_ipaddr_default == 0) {
				return "/";
			}
			return aat_ipaddr_to_string(this->extargs_ipaddr_default);
		default:
			return "/";
	}
}

/***************************************************************************
  函数名称：args_analyse_tools::aat_get_range
  功    能：返回取值范围/可选集合的显示文本，用于打印参数表的最后一列
  输入参数：无
  返 回 值：范围 "[a..b]"、集合 "a/b/c"，没有限制时返回 "/"
  说    明：
 ***************************************************************************/
const string args_analyse_tools::aat_get_range() const
{
	switch (this->extargs_type) {
		case ST_EXTARGS_TYPE::int_with_default:
		case ST_EXTARGS_TYPE::int_with_error:
			return "[" + aat_int_to_string(this->extargs_int_min) + ".." + aat_int_to_string(this->extargs_int_max) + "]";
		case ST_EXTARGS_TYPE::int_with_set_default:
		case ST_EXTARGS_TYPE::int_with_set_error:
			return aat_int_set_to_string(this->extargs_int_set);
		case ST_EXTARGS_TYPE::double_with_default:
		case ST_EXTARGS_TYPE::double_with_error:
			return "[" + aat_double_to_string(this->extargs_double_min) + ".." + aat_double_to_string(this->extargs_double_max) + "]";
		case ST_EXTARGS_TYPE::double_with_set_default:
		case ST_EXTARGS_TYPE::double_with_set_error:
			return aat_double_set_to_string(this->extargs_double_set);
		case ST_EXTARGS_TYPE::str_with_set_default:
		case ST_EXTARGS_TYPE::str_with_set_error:
			return aat_string_set_to_string(this->extargs_string_set);
		default:
			return "/";
	}
}

/***************************************************************************
  函数名称：args_analyse_tools::aat_get_info
  功    能：生成报错信息里 "(类型:..., ... 缺省:...)" 这一段
  输入参数：无
  返 回 值：形如 "(类型:int, 范围[0..65535] 缺省:12345)" 的字符串
  说    明：
 ***************************************************************************/
const string args_analyse_tools::aat_get_info() const
{
	string result = "(";
	switch (this->extargs_type) {
		case ST_EXTARGS_TYPE::boolean:
			result += "类型:bool";
			break;
		case ST_EXTARGS_TYPE::int_with_default:
		case ST_EXTARGS_TYPE::int_with_error:
			result += "类型:int, 范围[" + aat_int_to_string(this->extargs_int_min) + ".."
				+ aat_int_to_string(this->extargs_int_max) + "]";
			break;
		case ST_EXTARGS_TYPE::int_with_set_default:
		case ST_EXTARGS_TYPE::int_with_set_error:
			result += "类型:int, 可取值[" + aat_int_set_to_string(this->extargs_int_set) + "]";
			break;
		case ST_EXTARGS_TYPE::double_with_default:
		case ST_EXTARGS_TYPE::double_with_error:
			result += "类型:double, 范围[" + aat_general_to_string(this->extargs_double_min) + ".."
				+ aat_general_to_string(this->extargs_double_max) + "]";
			break;
		case ST_EXTARGS_TYPE::double_with_set_default:
		case ST_EXTARGS_TYPE::double_with_set_error:
			result += "类型:double, 可取值[" + aat_double_set_to_general_string(this->extargs_double_set) + "]";
			break;
		case ST_EXTARGS_TYPE::str:
			result += "类型:string";
			break;
		case ST_EXTARGS_TYPE::str_with_set_default:
		case ST_EXTARGS_TYPE::str_with_set_error:
			result += "类型:string, 可取值[" + aat_string_set_to_string(this->extargs_string_set) + "]";
			break;
		case ST_EXTARGS_TYPE::ipaddr_with_default:
		case ST_EXTARGS_TYPE::ipaddr_with_error:
			result += "类型:IP地址";
			break;
		default:
			result += "类型:undefined";
			break;
	}
	if (this->aat_has_default()) {
		switch (this->extargs_type) {
		case ST_EXTARGS_TYPE::int_with_default:
		case ST_EXTARGS_TYPE::int_with_set_default:
			result += " 缺省:" + aat_int_to_string(this->extargs_int_default);
			break;
		case ST_EXTARGS_TYPE::double_with_default:
		case ST_EXTARGS_TYPE::double_with_set_default:
			result += " 缺省:" + aat_general_to_string(this->extargs_double_default);
			break;
		case ST_EXTARGS_TYPE::ipaddr_with_default:
			result += " 缺省:" + this->extargs_string_default;
			break;
		default:
			result += " 缺省:" + this->extargs_string_default;
			break;
		}
	}
	result += ")";
	return result;
}

/***************************************************************************
  函数名称：args_analyse_tools::aat_switch_value
  功    能：解析并校验一个附加参数，结果写进对应的 value 成员
  输入参数：str：命令行里紧跟在参数名后面的那个字符串
  返 回 值：0 - 成功；-1 - 失败（错误信息已在本函数内打印）
  说    明：
 ***************************************************************************/
int args_analyse_tools::aat_switch_value(const char* const str)
{
	switch (this->extargs_type) {
		case ST_EXTARGS_TYPE::int_with_default:
		case ST_EXTARGS_TYPE::int_with_error:
		{
			int value = 0;
			if (aat_str_to_int(str, value)) {
				cout << "参数[" << this->args_name << "]的附加参数不是整数. " << this->aat_get_info() << endl;
				return -1;
			}
			if (value < this->extargs_int_min || value > this->extargs_int_max) {
				if (this->extargs_type == ST_EXTARGS_TYPE::int_with_error) {
					cout << "参数[" << this->args_name << "]的附加参数值(" << aat_int_to_string(value)
						<< ")非法. " << this->aat_get_info() << endl;
					return -1;
				}
				value = this->extargs_int_default;
			}
			this->extargs_int_value = value;
			break;
		}
		case ST_EXTARGS_TYPE::int_with_set_default:
		case ST_EXTARGS_TYPE::int_with_set_error:
		{
			int value = 0;
			if (aat_str_to_int(str, value)) {
				cout << "参数[" << this->args_name << "]的附加参数不是整数. " << this->aat_get_info() << endl;
				return -1;
			}
			if (!aat_in_set(this->extargs_int_set, INVALID_INT_VALUE_OF_SET, value)) {
				if (this->extargs_type == ST_EXTARGS_TYPE::int_with_set_error) {
					cout << "参数[" << this->args_name << "]的附加参数值(" << aat_int_to_string(value)
						<< ")非法. " << this->aat_get_info() << endl;
					return -1;
				}
				value = this->extargs_int_default;
			}
			this->extargs_int_value = value;
			break;
		}
		case ST_EXTARGS_TYPE::double_with_default:
		case ST_EXTARGS_TYPE::double_with_error:
		{
			double value = 0.0;
			if (aat_str_to_double(str, value)) {
				cout << "参数[" << this->args_name << "]的附加参数不是浮点数. " << this->aat_get_info() << endl;
				return -1;
			}
			if (value < this->extargs_double_min - DOUBLE_DELTA || value > this->extargs_double_max + DOUBLE_DELTA) {
				if (this->extargs_type == ST_EXTARGS_TYPE::double_with_error) {
					cout << "参数[" << this->args_name << "]的附加参数值(" << aat_general_to_string(value)
						<< ")非法. " << this->aat_get_info() << endl;
					return -1;
				}
				value = this->extargs_double_default;
			}
			this->extargs_double_value = value;
			break;
		}
		case ST_EXTARGS_TYPE::double_with_set_default:
		case ST_EXTARGS_TYPE::double_with_set_error:
		{
			double value = 0.0;
			if (aat_str_to_double(str, value)) {
				cout << "参数[" << this->args_name << "]的附加参数不是浮点数. " << this->aat_get_info() << endl;
				return -1;
			}
			if (!aat_in_set(this->extargs_double_set, INVALID_DOUBLE_VALUE_OF_SET, value)) {
				if (this->extargs_type == ST_EXTARGS_TYPE::double_with_set_error) {
					cout << "参数[" << this->args_name << "]的附加参数值(" << aat_general_to_string(value)
						<< ")非法. " << this->aat_get_info() << endl;
					return -1;
				}
				value = this->extargs_double_default;
			}
			this->extargs_double_value = value;
			break;
		}
		case ST_EXTARGS_TYPE::str:
			this->extargs_string_value = (str != NULL) ? string(str) : string("");
			break;
		case ST_EXTARGS_TYPE::str_with_set_default:
		case ST_EXTARGS_TYPE::str_with_set_error:
		{
			const string value = (str != NULL) ? string(str) : string("");
			if (!aat_in_set(this->extargs_string_set, string(""), value)) {
				if (this->extargs_type == ST_EXTARGS_TYPE::str_with_set_error) {
					cout << "参数[" << this->args_name << "]的附加参数值(" << value
						<< ")非法. " << this->aat_get_info() << endl;
					return -1;
				}
				this->extargs_string_value = this->extargs_string_default;
				break;
			}
			this->extargs_string_value = value;
			break;
		}
		case ST_EXTARGS_TYPE::ipaddr_with_default:
		case ST_EXTARGS_TYPE::ipaddr_with_error:
		{
			u_int value = 0;
			if (!aat_str_to_ipaddr(str, value)) {
				if (this->extargs_type == ST_EXTARGS_TYPE::ipaddr_with_error) {
						cout << "参数[" << this->args_name << "]的附加参数值(" << ((str != NULL) ? str : "")
						<< ")非法. " << this->aat_get_info() << endl;
					return -1;
				}
				value = this->extargs_ipaddr_default;
			}
			this->extargs_ipaddr_value = value;
			break;
		}
		default:
			break;
	}
	return 0;
}

/***************************************************************************
  函数名称：args_analyse_tools::aat_print
  功    能：按给定列宽打印本参数的一行
  输入参数：width_name    ：参数名列宽
  返 回 值：0
  说    明：
 ***************************************************************************/
int args_analyse_tools::aat_print(const int width_name, const int width_type, const int width_default,
	const int width_exists, const int width_value) const
{
	cout << ' ' << setw(width_name) << this->args_name
		<< ' ' << setw(width_type) << this->aat_get_type()
		<< ' ' << setw(width_default) << this->aat_get_default()
		<< ' ' << setw(width_exists) << this->existed()
		<< ' ' << setw(width_value) << this->aat_get_val()
		<< ' ' << this->aat_get_range() << endl;
	return 0;
}

/***************************************************************************
  函数名称：args_analyse_process
  功    能：
  输入参数：follow_up_args：是否有后续参数
  返 回 值：
  说    明：友元函数
 ***************************************************************************/
int args_analyse_process(const int argc, const char* const *const argv, args_analyse_tools* const args, const int follow_up_args)
{
	if (argc <= 0 || argv == NULL || args == NULL) {
		return -1;
	}
	int i = 1;
	for (; i < argc; i++) {
		const char* const token = argv[i];
		if (!aat_is_option(token)) {
			if (follow_up_args) {
				return i;
			}
			cout << "参数[" << token << "]格式非法(不是--开头的有效内容)." << endl;
			return -1;
		}
		int index = -1;
		for (int k = 0; args[k].extargs_type != ST_EXTARGS_TYPE::null; k++) {
			if (args[k].args_name == token) {
				index = k;
				break;
			}
		}
		if (index < 0) {
			cout << "参数[" << token << "]非法." << endl;
			return -1;
		}
		args_analyse_tools& cur = args[index];
		if (cur.args_existed) {
			cout << "参数[" << token << "]重复." << endl;
			return -1;
		}
		if (cur.extargs_num > 0) {
			if (i + 1 >= argc) {
				cout << "参数[" << token << "]的附加参数不足. " << cur.aat_get_info() << endl;
				return -1;
			}
			const char* const pvalue = argv[i + 1];
			if (aat_is_option(pvalue)) {
				cout << "参数[" << token << "]缺少附加参数. " << cur.aat_get_info() << endl;
				return -1;
			}
			if (cur.aat_switch_value(pvalue) < 0) {
				return -1;
			}
			i++;
		}
		cur.args_existed = 1;
	}
	return i;
}

/***************************************************************************
  函数名称：args_analyse_print
  功    能：
  输入参数：
  返 回 值：
  说    明：友元函数
 ***************************************************************************/
int args_analyse_print(const args_analyse_tools* const args)
{
	if (args == NULL) {
		return -1;
	}
	int width_name = (int)strlen("name");
	int width_type = (int)strlen("type");
	int width_default = (int)strlen("default");
	const int width_exists = (int)strlen("exists");
	int width_value = (int)strlen("value");
	int width_range = (int)strlen("range/set");

	for (int k = 0; args[k].extargs_type != ST_EXTARGS_TYPE::null; k++) {
		const int name_len = (int)args[k].args_name.length();
		const int type_len = (int)strlen(args[k].aat_get_type());
		const int default_len = (int)args[k].aat_get_default().length();
		const int value_len = (int)args[k].aat_get_val().length();
		const int range_len = (int)args[k].aat_get_range().length();
		if (name_len > width_name)		width_name = name_len;
		if (type_len > width_type)		width_type = type_len;
		if (default_len > width_default)	width_default = default_len;
		if (value_len > width_value)		width_value = value_len;
		if (range_len > width_range)		width_range = range_len;
	}

	const int total_width = 1 + width_name + 1 + width_type + 1 + width_default + 1
		+ width_exists + 1 + width_value + 1 + width_range + 1;

	cout << left;

	for (int n = 0; n < total_width; n++) {
		cout << '=';
	}
	cout << endl;

	cout << ' ' << setw(width_name) << "name"
		<< ' ' << setw(width_type) << "type"
		<< ' ' << setw(width_default) << "default"
		<< ' ' << setw(width_exists) << "exists"
		<< ' ' << setw(width_value) << "value"
		<< ' ' << "range/set" << endl;

	for (int n = 0; n < total_width; n++) {
		cout << '=';
	}
	cout << endl;

	for (int k = 0; args[k].extargs_type != ST_EXTARGS_TYPE::null; k++) {
		args[k].aat_print(width_name, width_type, width_default, width_exists, width_value);
	}

	for (int n = 0; n < total_width; n++) {
		cout << '=';
	}
	cout << endl;
	cout << endl;

	return 0;
}

#endif // !ENABLE_LIB_COMMON_TOOLS

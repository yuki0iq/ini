#pragma once
#include <stdio.h>
#include <string>
#include <map>
#include <exception>
using namespace std;

#define char_to_hex(h1, h2, c) h1 = c/16, h2 = c%16

/*
;Class for using INI files in your programs.
;test INI file
[group]
gparam1=gval1
gparam2=10.100000000000003 ;double with 15 signs precision

[group.subgroup.subsubgroup] ;this isnt a subgroup but a way how to imitate it
elems=2
i0=0
i1=1

;multiline separator between groups
;semicolon [;] as a comment symbol

[world.gamearray-1] ;a way to store arrays
elems=5 ;num of elems in array
i0=1
i1=1
i2=2
i3=3
i4=5

[a]
b=true

;result: (shown as a tree)
;vecParamGroup
;-group
;--gparam=gval1
;--gparam2=10.100000000000003
;-group.subgroup.subsubgroup
;--elems=2
;--i0=0
;--i1=1
;.....
;-a
;--b=true
*/
class INI
{
public:
	INI(const char *file = "config.ini");
	~INI();

	bool save();
	bool load();

	//caller MUST NOT delete or free returned pointer, nullptr returned if fails to find group.value
	std::string get(std::string group, std::string key);

	//set group.key with value
	void set(std::string group, std::string key, std::string value);

	//Unescape \XY symbols
	void ParseSymbols(std::string &str);

	//Escape some symbols
	void UnParseSymbols(std::string &str);
private:
	void TrimNewLine(char *str);
	void TrimComment(char *str);
	std::string GetGroupName(char *str);
	void ParseStr(const char *str, std::string &sParam, std::string &sValue);

	typedef std::map<std::string, std::string> map_str_str;
	typedef std::map<std::string, map_str_str> paramsmap;
	paramsmap params;
	const char *filename;
};


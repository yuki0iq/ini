#include "ini.h"

INI::INI(const char *file)
	: filename(file)
{
	if (!load())
	{
		printf("Error:\tINI file %s don't exist, used by another process,\n\tsecured or there is a write protection on disk.\n", filename);
		throw exception("Error: can't open INI file!");
	}
}

INI::~INI()
{
}

bool INI::save()
{
	FILE *pF = fopen(filename, "w");
	if (!pF) return false;
	bool bFirst = true;
	for (auto paramGroup : params)
	{
		if(!bFirst) fprintf(pF, "\n");
		bFirst = false;
		fprintf(pF, "[%s]", paramGroup.first.c_str());
		for (auto param : paramGroup.second)
		{
			UnParseSymbols(param.second);
			fprintf(pF, "\n%s=%s", param.first.c_str(), param.second.c_str());
			ParseSymbols(param.second);
		}
	}
	fclose(pF);
	return true;
}

bool INI::load()
{
	FILE *pF = fopen(filename, "r");
	if (!pF) return false;
	params.clear();

	const int STR_LEN = 1024;
	char str[STR_LEN];
	string sGroupName;
	map_str_str curGroup;

	while (!feof(pF))
	{
		fgets(str, STR_LEN - 1, pF);

		TrimNewLine(str);
		TrimComment(str);

		string sGroupName_t = GetGroupName(str);
		if (!sGroupName_t.empty())
		{
			if ( ! curGroup.empty())
			{
				params[sGroupName] = curGroup;
				curGroup.clear();
			}
			sGroupName = sGroupName_t;
			continue;
		}

		string sParam;
		string sValue;
		ParseStr(str, sParam, sValue);

		if (sParam.empty() || sValue.empty())
		{
			continue;
		}

		ParseSymbols(sValue);

		curGroup[sParam] = sValue;
	}
	if ( ! curGroup.empty())
	{
		params[sGroupName] = curGroup;
		curGroup.clear();
	}

	fclose(pF);
	return true;
}

string INI::get(string group, string key)
{
	return params[group][key];
}

void INI::set(string group, string key, string value)
{
	params[group][key] = value;
}

void INI::TrimNewLine(char * str)
{
	int iLen = strlen(str);
	if (iLen > 1)
	{
		if ('\r' == str[iLen - 2] && '\n' == str[iLen - 1])
		{
			str[iLen - 2] = '\0';
			return;
		}
	}
	if (iLen > 0)
	{
		if (('\n' == str[iLen - 1]) || ('\r' == str[iLen - 1]))
		{
			str[iLen - 1] = '\0';
			return;
		}
	}
}

void INI::TrimComment(char * str)
{
	char *pch = strchr(str, ';');
	if (NULL != pch)
	{
		*pch = '\0';
	}
}

string INI::GetGroupName(char * str)
{
	int iLen = strlen(str);
	if (iLen > 2)
	{
		if ('[' == str[0] && ']' == str[iLen - 1])
		{
			string sGroupName = str + 1;
			sGroupName.erase(iLen - 2);
			return sGroupName;
		}
	}
	return "";
}

void INI::ParseStr(const char * str, string & sParam, string & sValue)
{
	char *pch = strchr(const_cast<char*>(str), '=');
	if (NULL == pch)
	{
		return;
	}

	sParam = str;
	int iLen = pch - str;
	sParam.erase(iLen);
	sValue = pch + 1;
}

void INI::ParseSymbols(std::string & str)
{
	string temp;

	for (int i = 0; i < str.size(); ++i)
	{
		if (i + 2 < str.size() && str[i] == '\\')
		{
			char a, b;
			a = toupper(str[i + 1]);
			b = toupper(str[i + 2]);
			char c;
			if (a >= 0x30 && a <= 0x39) a -= 0x30;
			else if (a >= 0x41 && a <= 0x46) a -= 0x37;
			else continue;
			if (b >= 0x30 && b <= 0x39) b -= 0x30;
			else if (b >= 0x41 && b <= 0x46) b -= 0x37;
			else continue;
			c = a * 16 + b;
			temp += c;
			i += 2;
		}
		else
			temp += str[i];
	}

	str = temp;
}

//Unparse ONLY \x00 - \x1f, \x3b, \x3d, \x5b, \x5d, \x7f
void INI::UnParseSymbols(std::string & str)
{
	string temp;

	for (int i = 0; i < str.size(); ++i)
	{
		if (str[i] <= 0x1f ||
			str[i] == 0x3b ||
			str[i] == 0x3d ||
			str[i] == 0x5b ||
			str[i] == 0x3d ||
			str[i] == 0x7f)
		{
			char h1, h2;
			char_to_hex(h1, h2, str[i]);
			temp += '\\';
			temp += (h1<10?h1+'0':h1+'A');
			temp += (h2<10?h2+'0':h2+'A'-10);
		}
		else
			temp += str[i];
	}

	str = temp;
}

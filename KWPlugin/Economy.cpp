#pragma once
#include "Prebuild.h"
#include <string>
#include "CConfig.h"


class Economy
{
public:
	int static GetPlayerMoney(std::string uuid) {
		CConfig config;
		config.SetFilePath("Economy");
		string value = "";
		string error = "";
		config.GetValue("Money", uuid, value, error);
		return atoi(value.c_str());
	}

	int static GivePlayerMoney(std::string uuid, int count) {
		CConfig config;
		config.SetFilePath("Economy");
		string value = "";
		string error = "";
		config.GetValue("Money", uuid, value, error);
		int now=atoi(value.c_str()) + count;
		config.ModifyValue("Money", uuid, intToString(now), error);
		return now;		
	}


	vector<string> static SplitStr(string strtem, char a)
	{
		vector<string> strvec;

		string::size_type pos1, pos2;
		pos2 = strtem.find(a);
		pos1 = 0;
		while (string::npos != pos2)
		{
			strvec.push_back(strtem.substr(pos1, pos2 - pos1));

			pos1 = pos2 + 1;
			pos2 = strtem.find(a, pos1);
		}
		strvec.push_back(strtem.substr(pos1));
		return strvec;
	}

	static string intToString(int v)
	{
		char buf[32] = { 0 };
		snprintf(buf, sizeof(buf), "%u", v);

		string str = buf;
		return str;
	}
private:

};


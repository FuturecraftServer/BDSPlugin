#pragma once
#include "Prebuild.h"
#include "CConfig.cpp"
//#include <string>


class Economy
{
public:
	int static GetPlayerMoney(std::string uuid) {
		return CConfig::GetValueInt("Economy", "Money", uuid, 0);
	}

	int static GivePlayerMoney(std::string uuid, int count) {
		int value = CConfig::GetValueInt("Economy", "Money", uuid, 0);
		int now = value + count;
		CConfig::SetValueString("Economy", "Money", uuid, intToString(now));
		return now;
	}

	bool static SetPlayerMoney(std::string uuid, int count) {
		return CConfig::SetValueString("Economy", "Money", uuid, intToString(count));
	}

	int static RemovePlayerMoney(std::string uuid, int count) {
		int value = CConfig::GetValueInt("Economy", "Money", uuid, 0);
		int now = value - count;
		CConfig::SetValueString("Economy", "Money", uuid, intToString(now));
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


#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#include "Prebuild.h"
#include "CConfig.cpp"
#include "BDSAPI.hpp"
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

	void static DailySign(Player* player) {
		char tick[MAX_PATH];  //用于存储格式的时间
		struct tm t;     //tm结构指针
		time_t now;     //声明time_t类型变量
		time(&now);      //获取系统日期和时间
		localtime_s(&t, &now);   //获取当地日期和时间
		strftime(tick, _countof(tick), "%Y%m%d", &t);
		if (CConfig::GetValueString("Economy", "sign", player->getNameTag(), "0") == tick) {
			player->sendMsg("你今天已经签过到了!");
			return;
		}
		else {
			GivePlayerMoney(player->getNameTag(), 1);
			CConfig::SetValueString("Economy", "sign", player->getNameTag(), tick);
			player->sendMsg("签到成功! 1 金币 Get~");
		}
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
		return std::to_string(v);
	}
private:

};


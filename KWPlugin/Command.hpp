#pragma once
#include "Prebuild.h"
#include <string>
#include <vector>
#include <algorithm>// std::replace_if
#include "BDSAPI.hpp"
#include "Economy.cpp"

#include <time.h>

static std::map<std::string, Player*> onlinePlayers;
static std::map<std::string, std::string> NametoUuid;
using namespace std;
class Command {
public:

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

	string static intToString(int v)
	{
		char buf[64] = { 0 };
		snprintf(buf, sizeof(buf), "%u", v);

		string str = buf;
		return str;
	}

	bool static onConsoleSendCommand(std::string cmd) {
		vector<string> params = SplitStr(cmd, ' ');

		return !ProcessConsoleCommand(params);
	}

	bool static onPlayerSendCommand(std::string cmd, Player* player) {
		vector<string> params = SplitStr(cmd, ' ');
		return ProcessCommand(params, player);
	}



	bool static ProcessCommand(vector<string> param, Player* player) {
		if (param[0] == "/money") {
			player->sendMsg("你的余额为: §l§a" + intToString(Economy::GetPlayerMoney(player->getNameTag())));
		}
		else if (param[0] == "/pay") {
			if (param.size() != 3) {
				player->sendMsg("用法: /pay <玩家名> <金额>");
				return true;
			}
			int willgive = atoi(param[2].c_str());
			if (Economy::GetPlayerMoney(player->getNameTag()) < willgive) {
				player->sendMsg("你的余额为: §l§c" + intToString(Economy::GetPlayerMoney(player->getNameTag())));
				player->sendMsg("不足以支付 §l§c" + param[2]);
			}
			else {
				if (willgive <= 0) {
					player->sendMsg("金额输入有误 " + param[2]);
					return true;
				}
				Economy::GivePlayerMoney(param[1], willgive);
				Economy::RemovePlayerMoney(player->getNameTag(), willgive);
				player->sendMsg("成功给 §a§l" + param[1] + " §r金额 §a§l" + param[2]);
				player->sendMsg("你的余额为: §l§a" + intToString(Economy::GetPlayerMoney(player->getNameTag())));
				string sendoutuuid = NametoUuid[param[1]];
				if (sendoutuuid != "") {//玩家在线
					onlinePlayers[sendoutuuid]->sendMsg("玩家: " + player->getNameTag() + " 给你转账 §l§a" + param[2]);
					onlinePlayers[sendoutuuid]->sendMsg("你当前余额为: §l§a" + intToString(Economy::GetPlayerMoney(onlinePlayers[sendoutuuid]->getNameTag())));
				}
			}
		}
		else if (param[0] == "/tpa") {
			if (param.size() != 2) {
				player->sendMsg("用法: /tpa <玩家名>");
				return true;
			}
			string sendoutuuid = NametoUuid[param[1]];
			if (sendoutuuid != "") {//玩家在线
				CConfig::SetValueString("TPA", param[1], "from", player->getNameTag());
				CConfig::SetValueString("TPA", param[1], "time", intToString(time(NULL)));
				onlinePlayers[sendoutuuid]->sendMsg("玩家: " + player->getNameTag() + " 请求tpa到你这里 §l§a");
				onlinePlayers[sendoutuuid]->sendMsg("输入 §l§a/tpayes §r即可同意请求");
				onlinePlayers[sendoutuuid]->sendMsg("输入 §l§c/tpano §r即可不同意请求");
				onlinePlayers[sendoutuuid]->sendMsg("该请求 §l§a60秒 §r后过期,到时候可以忽略");
				player->sendMsg("发送请求§l§a成功!§r在60秒内对方可以接受你的请求");
			}
			else {
				player->sendMsg("玩家: " + param[1] + " 不在线!");
				return true;
			}
		}
		else if (param[0] == "/tpayes") {
			if (atoi(CConfig::GetValueString("TPA", player->getNameTag(), "time", "NaN").c_str()) < time(NULL) - 61)
			{

				player->sendMsg("§l§c所有请求已过期!");
				return true;
			}
			string from = CConfig::GetValueString("TPA", player->getNameTag(), "from", "");
			string sendoutuuid = NametoUuid[from];
			if (sendoutuuid != "") {//玩家在线
				CConfig::SetValueString("TPA", player->getNameTag(), "time", intToString(0));
				runcmd("tp " + from + " " + player->getNameTag());
				onlinePlayers[sendoutuuid]->sendMsg("成功TPA到" + player->getNameTag());
				player->sendMsg("玩家 " + from + " 已成功TPA到此处");
			}
			else {
				player->sendMsg("§l§c请求发起者已下线!");
			}
		}
		else if (param[0] == "/tpano") {
			string from = CConfig::GetValueString("TPA", player->getNameTag(), "from", "");
			string sendoutuuid = NametoUuid[from];
			if (sendoutuuid != "") {//玩家在线
				CConfig::SetValueString("TPA", player->getNameTag(), "time", intToString(0));
				onlinePlayers[sendoutuuid]->sendMsg("玩家: " + player->getNameTag() + " 拒绝了你的TPA请求");
				player->sendMsg("成功拒绝 " + from + " 的TPA请求");
			}
			else {
				player->sendMsg("§l§c请求发起者已下线!");
			}
		}
		else {
			return false;
		}
		return true;
	}

	bool static ProcessConsoleCommand(vector<string> param) {
		if (param[0] == "money") {
			if (param.size() == 2) {
				cout << u8"他的余额为: " << intToString(Economy::GetPlayerMoney(param[1])) << endl;
			}
			else {
				cout << u8"用法: money <玩家名>" << endl;
			}
		}
		else if (param[0] == "pay") {
			if (param.size() == 3) {
				cout << u8"给钱成功! 他的余额目前为: " << Economy::GivePlayerMoney(param[1], atoi(param[2].c_str())) << endl;
				string sendoutuuid = NametoUuid[param[1]];
				if (sendoutuuid != "") {//玩家在线
					onlinePlayers[sendoutuuid]->sendMsg("管理员 给你转账 §l§a" + param[2]);
					onlinePlayers[sendoutuuid]->sendMsg("你当前余额为: §l§a" + intToString(Economy::GetPlayerMoney(onlinePlayers[sendoutuuid]->getNameTag())));
				}
			}
			else {
				cout << u8"用法: pay <玩家名> <数量>" << endl;
			}
		}
		else {
			return false;
		}
		return true;
	}

};
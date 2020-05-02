#pragma once
#include "Prebuild.h" //Prebuild Header
#include "CConfig.cpp"
#include "BDSAPI.hpp"
using namespace std;

class Guild {
public:
	string static getPlayerGuildName(string player) {
		string ret = CConfig::GetValueString("Guild", "Player", player, "NaN");

		return ret;
	}

	bool static isInGuild(string player, string guild = "NaN") {
		if (guild == "NaN") {
			string gname = CConfig::GetValueString("Guild", "Player", player, "NaN");
			return (isGuildSet(gname) && gname != "NaN");
		}
		else {
			string gname = CConfig::GetValueString("Guild", "Player", player, "NaN");
			return gname == guild;
		}
	}

	bool static isAdmin(string player, string guild) {
		return GetAdmin(guild) == player;
	}

	void static CreateGuild(string name, string admin) {
		CConfig::SetValueString("Guild", name, "admin", admin);
		CConfig::SetValueString("Guild", name, "Player", admin);
		CConfig::SetValueString("Guild", "Player", admin, name);
	}

	bool static isGuildSet(string name) {
		return CConfig::GetValueString("Guild", name, "admin", "NaN") == "NaN" ? false : true;
	}

	string static PlayerInWhich(string player) {
		return CConfig::GetValueString("Guild", "Player", player, "NaN");
	}

	string static GetPlayers(string guild) {
		return CConfig::GetValueString("Guild", guild, "Player", "NaN");
	}

	void static JoinGuild(string name, string player) {
		CConfig::SetValueString("Guild", "Player", player, name);
		CConfig::SetValueString("Guild", name, "Player", CConfig::GetValueString("Guild", name, "Player", "NaN") + player + ",");
	}

	void static RequestJoin(string name, string player) {
		CConfig::SetValueString("Guild", "Request", name, player);
	}

	void static AccecptJoin(string name, Player* player) {
		if (isAdmin(player->getRealNameTag(), name)) {
			string requester = CConfig::GetValueString("Guild", "Request", name, "NaN");
			if (requester == "NaN") {
				player->sendMsg("没有请求!");
			}
			else {
				if (isInGuild(requester)) {
					player->sendMsg("同意失败,对方已经加入公会");
					return;
				}
				JoinGuild(name, requester);
				player->sendMsg("同意成功");
			}
		}
		else {
			player->sendMsg("你不是公会管理员!");
		}
	}

	string static GetAdmin(string name) {
		return CConfig::GetValueString("Guild", name, "admin", "NaN");
	}

	void static RemoveGuild(string name, Player* player) {
		if (isAdmin(player->getRealNameTag(), name)) CConfig::SetValueString("Guild", name, "admin", "NaN");
		player->sendMsg("成功解散公会!");
	}

	void static ExitGuild(string name, Player* player) {
		string guild = PlayerInWhich(player->getRealNameTag());
		if (guild != "NaN") {
			CConfig::SetValueString("Guild", guild, "Player", replace_all_distinct(GetPlayers(guild), player->getRealNameTag() + ",", ""));
			CConfig::SetValueString("Guild", "Player", player->getRealNameTag(), "NaN");

			player->sendMsg("成功退出公会!");
		}
		else {
			player->sendMsg("你还未加入公会!");
		}
	}
};
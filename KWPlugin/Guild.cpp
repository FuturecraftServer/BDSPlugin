#pragma once
#include "Prebuild.h"
#include "CConfig.cpp"
#include "BDSAPI.hpp"
using namespace std;

class Guild {
public:
	string static getPlayerGuildName(string player) {
		return CConfig::GetValueString("Guild", "Player", player, "NaN");
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
		CConfig::SetValueString("Guild", "Player", admin, name);
	}

	bool static isGuildSet(string name) {
		return CConfig::GetValueString("Guild", name, "admin", "NaN") == "NaN" ? false : true;
	}

	string static PlayerInWhich(string player) {
		return CConfig::GetValueString("Guild", "Player", player, "NaN");
	}

	void static JoinGuild(string name, string player) {
		CConfig::SetValueString("Guild", "Player", player, name);
	}

	void static RequestJoin(string name, string player) {
		CConfig::SetValueString("Guild", "Request", name, player);
	}

	void static AccecptJoin(string name, Player* player) {
		if (isAdmin(player->getNameTag(), name)) {
			string requester = CConfig::GetValueString("Guild", "Request", name, "NaN");
			if (requester == "NaN") {
				player->sendMsg("û������!");
			}
			else {
				if (isInGuild(requester)) {
					player->sendMsg("ͬ��ʧ��,�Է��Ѿ����빫��");
					return;
				}
				JoinGuild(name, requester);
				player->sendMsg("ͬ��ɹ�");
			}
		}
		else {
			player->sendMsg("�㲻�ǹ������Ա!");
		}
	}

	string static GetAdmin(string name) {
		return CConfig::GetValueString("Guild", name, "admin", "NaN");
	}

	void static RemoveGuild(string name, Player* player) {
		if (isAdmin(player->getNameTag(), name)) CConfig::SetValueString("Guild", name, "admin", "NaN");
		player->sendMsg("�ɹ���ɢ����!");
	}

	void static ExitGuild(string name, Player* player) {
		if (PlayerInWhich(player->getNameTag()) != "NaN") {
			CConfig::SetValueString("Guild", "Player", player->getNameTag(), "NaN");
			player->sendMsg("�ɹ��˳�����!");
		}
		else {
			player->sendMsg("�㻹δ���빫��!");
		}
	}
};
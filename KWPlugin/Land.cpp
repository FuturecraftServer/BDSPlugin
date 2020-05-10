#pragma once
#include "Prebuild.h" //Prebuild Header
#include "CConfig.cpp"
#include "BDSAPI.hpp"
#include "Guild.cpp"
#include "Economy.cpp"
using namespace std;
class Land {
public:
	bool static isLandOwned(string landid) {
		return CConfig::GetValueString("Land", landid, "guild", "NaN") != "NaN";
	}

	bool static canLandSpawn(string landid, int mobid) {
		string list = CConfig::GetValueString("Land", landid, "nospawn", "NaN");
		//cout <<" ["<< landid<<"] Spawn is " << list;
		if (list == "all") return false;
		if (list == "NaN") return true;
		if (list.find(std::to_string(mobid) + ",")== list.npos) {
			return true;
		}
		else {
			return false;
		}
	}

	string static getLandName(string landid) {
		return CConfig::GetValueString("Land", landid, "name", "未命名领地");
	}

	void static setLandName(string landid, string name) {
		CConfig::SetValueString("Land", landid, "name", name);
	}

	string static getLandOwner(string landid) {
		return CConfig::GetValueString("Land", landid, "guild", "NaN");
	}

	bool static canLandPvP(string landid, Player* attacker, Player* attackon) {
		if (CConfig::GetValueInt("Land", landid, "protectnew", 0) == 1 && Economy::GetPlayerMoney(attackon->getRealNameTag()) >= 50) {
			return true;
		}
		if (CConfig::GetValueInt("Land", landid, "pvp", 1) == 0) return false;
		if (Guild::isInGuild(attackon->getRealNameTag(), CConfig::GetValueString("Land", landid, "guild", AdminGuild))) return false;
		return true;
	}

	void static GiveLand(string landid, string guild) {
		CConfig::SetValueString("Land", landid, "guild", guild);
		//CConfig::SetValueString("Land", landid, "pvp", "0");
		CConfig::SetValueString("Land", landid, "modblock", "0");
	}

	bool static canLandModifyBlock(string landid, string breaker) {
		if (Guild::isInGuild(breaker, CConfig::GetValueString("Land", landid, "guild", AdminGuild)) || CConfig::GetValueInt("Land", landid, "modblock", 0) == 1 || !Land::isLandOwned(landid)) return true;
		return false;
	}

	string static BlockChunckId(BPos3* positon) {
		int xc = positon->x / 16;
		int zc = positon->z / 16;
		return intToString(xc) + "," + intToString(zc);
	}
	string static PlayerChunckId(Vec3* positon) {
		int xc = positon->x / 16;
		int zc = positon->z / 16;
		return intToString(xc) + "," + intToString(zc);
	}
private:
	string static intToString(int v)
	{
		return std::to_string(v);
	}
};
#pragma once
#include"BDSAPI.hpp"
#include "CConfig.cpp"
class AwardBox {
public:
	bool static isAwardBox(BlockPos* pos) {
		return CConfig::GetValueInt("AwardBox", pos->getPosition()->toNormalString(), "coin", 0);
	}

	int static getAward(BlockPos* pos, Player* player) {
		cout << CConfig::GetValueString("AwardBox", pos->getPosition()->toNormalString(), "tp", "0") << endl;
		if (CConfig::GetValueString("AwardBox", pos->getPosition()->toNormalString(), "tp", "0") != "0") {
			runcmd("tp " + player->getRealNameTag() + " "+ CConfig::GetValueString("AwardBox", pos->getPosition()->toNormalString(), "tp", "399 70 -61"));
		}
		if (CConfig::GetValueString("AwardBox", pos->getPosition()->toNormalString(), "type", "multi") == "once") {
			if (CConfig::GetValueInt("AwardBox", "Get", player->getRealNameTag() + pos->getPosition()->toNormalString(), 0) == 0) {
				CConfig::SetValueString("AwardBox", "Get", player->getRealNameTag() + pos->getPosition()->toNormalString(), "1");
				return CConfig::GetValueInt("AwardBox", pos->getPosition()->toNormalString(), "coin", 0);
			}
			else {
				return 0;
			}
		}
		else {
			return CConfig::GetValueInt("AwardBox", pos->getPosition()->toNormalString(), "coin", 0);
		}

	}

	void static RequestTP(Player* player) {
		CConfig::SetValueString("AwardBox", "RequestTP", player->getRealNameTag(), player->getPos()->toNormalString());
		player->sendMsg("Çëµã»÷½±ÀøÏä");
	}

	bool static isRequestTP(Player* player) {
		return !(CConfig::GetValueString("AwardBox", "RequestTP", player->getRealNameTag(), "0") == "0");
	}

	void static SetTP(BlockPos* pos, Player* player) {
		CConfig::SetValueString("AwardBox", pos->getPosition()->toNormalString(), "tp", CConfig::GetValueString("AwardBox", "RequestTP", player->getRealNameTag(), "0"));
		CConfig::SetValueString("AwardBox", "RequestTP", player->getRealNameTag(), "0");
	}

	void static SetAwardBox(BlockPos* pos, Player* player) {
		if (CConfig::GetValueString("AwardBox", "Request", player->getRealNameTag() + "_type", "mult") == "once") {
			CConfig::SetValueString("AwardBox", pos->getPosition()->toNormalString(), "type", "once");
		}
		CConfig::SetValueString("AwardBox", pos->getPosition()->toNormalString(), "coin", CConfig::GetValueString("AwardBox", "Request", player->getRealNameTag(), "0"));
		CConfig::SetValueString("AwardBox", "Request", player->getRealNameTag(), "0");
	}

	void static RequestSet(Player* player, string count, bool isonce) {
		CConfig::SetValueString("AwardBox", "Request", player->getRealNameTag(), count);
		CConfig::SetValueString("AwardBox", "Request", player->getRealNameTag() + "_type", isonce ? "once" : "mult");
		player->sendMsg("Çëµã»÷½±ÀøÏä!");
	}

	bool static isRequestSetBox(Player* player) {
		return !(CConfig::GetValueString("AwardBox", "Request", player->getRealNameTag(), "0") == "0");
	}
};
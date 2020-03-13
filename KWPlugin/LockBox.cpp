#pragma once
#include "Prebuild.h"
#include "BDSAPI.hpp"
#include "CConfig.cpp"


class LockBox {
public:
	bool static HavePermission(Player* player, BlockPos* blockpos) {
		string owner = CConfig::GetValueString("LockBox", blockpos->getPosition()->toNormalString(), "owner", "NaN");
		if (owner == "NaN") return true;
		if (player->getNameTag() == owner) {
			return true;
		}
		else {
			return false;
		}
	}

	void static SetPermission(Player* player, BlockPos* blockpos) {
		CConfig::SetValueString("LockBox", "Request", player->getNameTag(), "false");
		CConfig::SetValueString("LockBox", blockpos->getPosition()->toNormalString(), "owner", player->getNameTag());
	}

	bool static isRequestLockBox(Player* player) {
		if (CConfig::GetValueString("LockBox", "Request", player->getNameTag(), "false") == "true") {
			return true;
		}
		else {
			return false;
		}
	}

	bool static CheckDropper(Player* player, BlockPos* blockpos) {
		//Ö±½Ó½ûµô!
		return  false;
		string position = blockpos->getPosition()->toNormalString(0, 2, 0);
		string owner = CConfig::GetValueString("LockBox", position, "owner", "NaN");
		if (owner == "NaN") return true;
		if (player->getNameTag() == owner) {
			return true;
		}
		else {
			return false;
		}
	}

	void static RequestLockBox(Player* player) {
		CConfig::SetValueString("LockBox", "Request", player->getNameTag(), "true");
	}
private:

};
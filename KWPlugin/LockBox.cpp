#pragma once
#include "Prebuild.h"
#include "BDSAPI.hpp"
#include "CConfig.cpp"


class LockBox {
public:
	bool static HavePermission(Player* player, BlockPos* blockpos) {
		string owner = CConfig::GetValueString("LockBox", blockpos->getPosition()->toNormalString(), "owner", "NaN");
		if (owner == "NaN") return true;
		if (player->getRealNameTag() == owner) {
			return true;
		}
		else {
			return false;
		}
	}

	void static SetPermission(Player* player, BlockPos* blockpos) {
		CConfig::SetValueString("LockBox", "Request", player->getRealNameTag(), "false");
		CConfig::SetValueString("LockBox", blockpos->getPosition()->toNormalString(), "owner", player->getRealNameTag());
	}

	bool static isRequestLockBox(Player* player) {
		if (CConfig::GetValueString("LockBox", "Request", player->getRealNameTag(), "false") == "true") {
			return true;
		}
		else {
			return false;
		}
	}

	bool static CheckDropper(BlockPos* blockpos) {
		string position = blockpos->getPosition()->toNormalString(0, 1, 0);
		string owner = CConfig::GetValueString("LockBox", position, "owner", "NaN");
		if (owner == "NaN") return true;
		return false;
		/*
		if (player->getRealNameTag() == owner) {
			return true;
		}
		else {
			return false;
		}
		*/
	}

	void static RequestLockBox(Player* player) {
		CConfig::SetValueString("LockBox", "Request", player->getRealNameTag(), "true");
	}
private:

};
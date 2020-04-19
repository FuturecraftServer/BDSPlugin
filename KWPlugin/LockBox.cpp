#pragma once
#include "Prebuild.h" //Prebuild Header
#include "BDSAPI.hpp"
#include "CConfig.cpp"


class LockBox {
public:
	bool static HavePermission(string player, string blockpos) {
		string owner = CConfig::GetValueString("LockBox", blockpos, "owner", "NaN");
		if (owner == "NaN") return true;
		if (player == owner) {
			return true;
		}
		else {
			return false;
		}
	}

	string static GetOwner(string blockpos) {
		return CConfig::GetValueString("LockBox", blockpos, "owner", "NaN");
	}

	void static SetPermission(string player, string blockpos) {
		CConfig::SetValueString("LockBox", "Request", player, "false");
		CConfig::SetValueString("LockBox", blockpos, "owner", player);
	}

	void static RemoveLockBox(string blockpos) {
		CConfig::SetValueString("LockBox", blockpos, "owner", "NaN");
	}

	bool static isRequestLockBox(string player) {
		return CConfig::GetValueString("LockBox", "Request", player, "false") == "true";
	}

	bool static isRequestUnLockBox(string player) {
		return CConfig::GetValueString("LockBox", "RequestUn", player, "false") == "true";
	}

	bool static CheckDropper(BlockPos* blockpos) {
		string position = blockpos->getPosition()->toNormalString(0, 1, 0);
		string owner = CConfig::GetValueString("LockBox", position, "owner", "NaN");
		if (owner == "NaN") return true;
		return false;
	}

	bool static isLockBox(string blockpos) {
		return CConfig::GetValueString("LockBox", blockpos, "owner", "NaN") != "NaN";
	}

	void static RequestLockBox(string player) {
		CConfig::SetValueString("LockBox", "Request", player, "true");
	}

	void static RequestUnLockBox(string player) {
		CConfig::SetValueString("LockBox", "RequestUn", player, "true");
	}
private:

};
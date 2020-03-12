#include "BDSAPI.hpp"
#include "CConfig.cpp"

class LockBox {
public:
	bool HavePermission(Player* player, BlockPos* blockpos) {
		string owner = CConfig::GetValueString("LockBox", blockpos->getPosition()->toNormalString(), "player", "NaN");
		if (owner == "NaN") return true;
		if (player->getNameTag() == owner) {
			return true;
		}
		else {
			return false;
		}
	}
};
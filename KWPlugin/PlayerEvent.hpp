#include "Prebuild.h"
#include "BDSAPI.hpp"
#include "Command.hpp"

class PlayerEvent {
public:
	bool static UseItem(Player* player, ItemStack* item, BlockPos* pBlkpos, Block* pBlk) {
		return true;
	}

	bool static PlaceBlock(Player* player, const Block* block, BlockPos* position) {
		return true;
	}

	bool static BreakBlock(Player* player, const Block* block, BlockSource* blocksource) {
		return true;

	}

	bool static ReadyOpenBox(Player* player, BlockSource* blocksource, BlockPos* blockposition) {
		if ()
		return true;
	}

	bool static ReadyOpenBusket(Player* player, BlockSource* blocksource, BlockPos* blockposition) {
		return true;
	}

	void static CloseBox(Player* player, BlockPos* blockposition) {

	}

	void static CloseBusket(Player* player, BlockPos* blockposition) {
	}

	void static Spawn(Player* player) {
	}

	bool static Command(Player* player, std::string command) {
		if (Command::onPlayerSendCommand(command,player)) return false; //问为什么,要拦截下来他本来的Process
		return true;
	}

	void static Exit(Player* player) {
	}

	bool static Attack(Player* player, Actor* actor) {
		return true;
	}
};

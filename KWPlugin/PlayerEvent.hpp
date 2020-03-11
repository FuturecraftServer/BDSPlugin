#include "Prebuild.h"
#include "BDSAPI.hpp"
class PlayerEvent {
public:
	bool static UseItem(Player* player, ItemStack* item, BlockPos* pBlkpos, Block* pBlk) {
		std::cout << "Player: " << player->getUuid() << std::endl << "Item: " << item->getId() << "(" << item->getName() << ")" << std::endl;
		return true;
	}

	bool static PlaceBlock(Player* player, const Block* block, BlockPos* position) {
		std::cout << "Player: " << player->getUuid() << std::endl << "Placing Block: " << block->getLegacyBlock()->getBlockItemID() << "(" << block->getLegacyBlock()->getFullName() << ")" << std::endl;
		return true;
	}

	bool static BreakBlock(Player* player, const Block* block, BlockSource* blocksource) {
		std::cout << "Player: " << player->getUuid() << std::endl << "Breaking Block: " << block->getLegacyBlock()->getBlockItemID() << "(" << block->getLegacyBlock()->getFullName() << ")" << std::endl;
		return true;

	}

	bool static ReadyOpenBox(Player* player, BlockSource* blocksource, BlockPos* blockposition) {
		std::cout << "Player: " << player->getUuid() << std::endl << "Trying Open box on: " << blockposition->getPosition()->x << "," << blockposition->getPosition()->y << "," << blockposition->getPosition()->z << "," << std::endl;
		return true;

	}

	bool static ReadyOpenBusket(Player* player, BlockSource* blocksource, BlockPos* blockposition) {
		std::cout << "Player: " << player->getUuid() << std::endl << "Trying Open busket on: " << blockposition->getPosition()->x << "," << blockposition->getPosition()->y << "," << blockposition->getPosition()->z << "," << std::endl;
		return true;

	}

	void static CloseBox(Player* player, BlockPos* blockposition) {
		std::cout << "Player: " << player->getUuid() << std::endl << "Close box on: " << blockposition->getPosition()->x << "," << blockposition->getPosition()->y << "," << blockposition->getPosition()->z << std::endl;

	}

	void static CloseBusket(Player* player, BlockPos* blockposition) {
		std::cout << "Player: " << player->getUuid() << std::endl << "Close busket on: " << blockposition->getPosition()->x << "," << blockposition->getPosition()->y << "," << blockposition->getPosition()->z << std::endl;
	}

	void static Spawn(Player* player) {
		std::cout << "Player: " << player->getUuid() << std::endl << "Spawn on: " << player->getPos()->x << player->getPos()->y << "," << player->getPos()->z << std::endl;
	}

	bool static Command(Player* player, std::string command) {
		std::cout << "Player: " << player->getUuid() << std::endl << "Send Command : " << command << std::endl;
		return true;
	}

	void static Exit(Player* player) {
		std::cout << "Player: " << player->getUuid() << std::endl << "Exit" << std::endl;
	}

	bool static Attack(Player* player, Actor* actor) {
		std::cout << "Player: " << player->getUuid() << std::endl << "Attack: " << actor->getNameTag() << "(" << actor->getNameTag() << ")" << std::endl;
		return true;
	}
};

#include "Prebuild.h"
#include "BDSAPI.hpp"
#include "Command.hpp"

class PlayerEvent {
public:
	bool static UseItem(Player* player, ItemStack* item, BlockPos* pBlkpos, Block* pBlk) {
		return true;
	}

	bool static PlaceBlock(Player* player, short blockid, BlockPos* position) {
		if (blockid == 410) {
			if (!LockBox::CheckDropper(player, position)) {
				player->sendMsg("���޷��ڴ˴�����©��,��Ϊ����������������");
				return false;
			}
		}

		return true;
	}

	bool static BreakBlock(Player* player, const Block* block, BlockSource* blocksource, BlockPos* blockpos) {
		if (block->getLegacyBlock()->getBlockItemID() == 54) {
			if (!LockBox::HavePermission(player, blockpos)) {
				player->sendMsg("��û��Ȩ���ƻ�������!");
				return false;
			}
		}
		return true;

	}

	bool static ReadyOpenBox(Player* player, BlockSource* blocksource, BlockPos* blockposition) {
		if (!LockBox::HavePermission(player, blockposition)) {
			player->sendMsg("��û��Ȩ�޴򿪴�����!");
			return false;
		}
		if (LockBox::isRequestLockBox(player)) {
			LockBox::SetPermission(player, blockposition);
			player->sendMsg("���ѳɹ�����!");
			return false;
		}
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
		//if (Guild::isInGuild(player->getNameTag())) player->reName("[" + Guild::PlayerInWhich(player->getNameTag()) + "] " + player->getNameTag());
	}

	// ����������
	static bool reNameByUuid(std::string uuid, std::string newName) {
		bool ret = false;
		Player* taget = onlinePlayers[uuid];
		if (taget != NULL) {
			taget->reName(newName);
			ret = true;
		}
		return ret;
	}

	bool static Command(Player* player, std::string command) {
		if (Command::onPlayerSendCommand(command, player)) return false; //��Ϊʲô,Ҫ����������������Process
		return true;
	}

	void static Exit(Player* player) {
	}

	bool static Attack(Player* player, Actor* actor) {
		return true;
	}

	bool static Chat(Player* player, string* chat) {
		string nametag = player->getNameTag();
		if (Guild::isInGuild(nametag)) {
			cout << "[" + Guild::getPlayerGuildName(nametag) + "]<" + nametag + "> " + *chat << endl;
			runcmd("tellraw @a {\"rawtext\":[{\"text\":\"[" + Guild::getPlayerGuildName(nametag) + "]<" + nametag + "> " + *chat + "\"}]}");
		}
		else {
			cout << "<" + nametag + "> " + *chat << endl;
			runcmd("tellraw @a {\"rawtext\":[{\"text\":\"<" + nametag + "> " + *chat + "\"}]}");
		}
	}
};

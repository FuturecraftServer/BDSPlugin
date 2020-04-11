#pragma once
#include "BDSAPI.hpp"
#include "Command.cpp"
#include "RPG.cpp"
#include "Land.cpp"
#include "ChestShop.cpp"



class PlayerEvent {
public:
	bool static UseItem(Player* player, ItemStack* item, BlockPos* blockpos, Block* pBlk) {
		return true;
	}

	bool static PlaceFire(Player* player, BlockPos* blockpos) {
		if (Guild::isInGuild(player->getRealNameTag(), Land::getLandOwner(Land::BlockChunckId(blockpos->getPosition())))) {
			return true;
		}
		else {
			return false;
		}
	}


	void static SelectForm(Player* player, UINT fid, string selected) {
		if (selected == "null") {
			selected = "-1";
		}
		if (ChestShop::isRequestBuyForm(player)) {
			ChestShop::realbuy(player, atoi(selected.c_str()));
		}
	}

	bool static ChestInput(Player* player, ItemStack* item, BlockPos* blkpos) {
		if (item->getId() == 0) return true;
		if (ChestShop::isChestShop(blkpos)) {
			int ret = ChestShop::SellItem(item, player, item->getStackSize());
			if (ret == 1) {
				return false;
			}
			else if (ret == -1) {
				player->sendMsg("�ݲ�֧���۳� " + item->getName() + " * " + std::to_string(item->getStackSize()));
				return true;
			}
			else if (ret == 2) {
				player->sendMsg("�����۳�����������");
				return true;
			}
		}
		return true;
	}

	bool static BreakItemFrame(Player* player, BlockPos* position) {
		cout << "Player: " << player << " Break Frame " << position->getPosition()->toNormalString() << endl;
		return false;
	}

	bool static PlaceBlock(Player* player, short blockid, BlockPos* position) {
		if (!Land::canLandModifyBlock(Land::BlockChunckId(position->getPosition()), player)) {
			player->sendMsg("���޷��ڴ���ط��÷���!");
			return false;
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
		if (!Land::canLandModifyBlock(Land::BlockChunckId(blockpos->getPosition()), player)) {
			player->sendMsg("���޷��ڴ�����ƻ�����!");
			return false;
		}
		return true;

	}

	bool static ReadyOpenBox(Player* player, BlockSource* blocksource, BlockPos* blockposition) {
		if (ChestShop::isRequestSetChestShop(player)) {
			ChestShop::setChestShop(player, blockposition);
			player->sendMsg("Set Done!");
		}
		if (AwardBox::isRequestSetBox(player)) {
			AwardBox::SetAwardBox(blockposition, player);
			player->sendMsg("Box Setted!");
			return false;
		}
		if (AwardBox::isRequestTP(player)) {
			AwardBox::SetTP(blockposition, player);
			player->sendMsg("TP Setted!");
			return false;
		}
		if (AwardBox::isAwardBox(blockposition)) {
			int aw = AwardBox::getAward(blockposition, player);
			Economy::GivePlayerMoney(player->getRealNameTag(), aw);
			player->sendMsg("���ѻ�� " + std::to_string(aw));
			return false;
		}

		if (ChestShop::isChestShop(blockposition)) {
			return true;
		}
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
		std::ifstream in(CConfig::GetPluginPath() + "Annoncement.txt");
		std::ostringstream tmp;
		tmp << in.rdbuf();
		std::string str = tmp.str();

		//player->sendMsg(str);    //������
		sendMsgForm((VA)player,str,"����");
		if (CConfig::GetValueString("Player", "Darkroom", player->getRealNameTag(), "false") == "true") {
			runcmd("tp " + player->getRealNameTag() + " "+CConfig::GetValueString("Settings","Settings","darkroom","0 0 0"));
			player->sendMsg("���ѱ�����С���ݷ�˼.");
		}
	}

	unsigned static sendMsgForm(VA pPlayer, std::string content, std::string title = "��Ϣ") {
		if (pPlayer == 0)
			return 0;
		std::string orig = "{\"content\": \"" + content + "\",\"title\": \"" + title + "\"}";
		return sendForm(pPlayer, orig);
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
		if (actor->getEntityTypeId() == 1)
			if (Land::canLandPvP(Land::PlayerChunckId(actor->getPos()), player, (Player*)actor)) {
				return true;
			}
			else {
				player->sendMsg("���޷��ڴ���ع�����!");
				return false;
			}
		return true;
	}

	void static Chat(Player* player, string* chat) {
		string nametag = player->getRealNameTag();
		if (Guild::isInGuild(nametag)) {
			cout << "[" + Guild::getPlayerGuildName(nametag) + "]<" + nametag + "> " + *chat << endl;
			runcmd("tellraw @a {\"rawtext\":[{\"text\":\"[" + Guild::getPlayerGuildName(nametag) + "] " + nametag + " >>> " + *chat + "\"}]}");
		}
		else {
			cout << "<" + nametag + "> " + *chat << endl;
			runcmd("tellraw @a {\"rawtext\":[{\"text\":\"" + nametag + " >>> " + *chat + "\"}]}");
		}
	}
};

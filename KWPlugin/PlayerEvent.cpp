#pragma once
#include "Prebuild.h" //Prebuild Header
#include "BDSAPI.hpp"
#include "Command.cpp"
#include "RPG.cpp"
#include "Land.cpp"
#include "ChestShop.cpp"

static std::map<std::string, std::string> PlayerLastLandId;
static std::map<std::string, bool> PlayerLastinLand;

class PlayerEvent {
public:
	bool static UseItem(Player* player, ItemStack* item, BlockPos* blockpos, Block* pBlk) {
		if (item->getId() == 383) {
			if (item->getAuxValue() == 83) {
				runcmd("tp " + player->getRealNameTag() + " " + CConfig::GetValueString("Settings", "Settings", "darkroom", "0 0 0"));
				CConfig::SetValueString("Player", "Darkroom", player->getRealNameTag(), "true");
				string sendoutuuid = NametoUuid[player->getRealNameTag()];
				if (sendoutuuid != "") {//玩家在线	
					onlinePlayers[sendoutuuid]->sendMsg("你被关到了小黑屋");
				}
				return false;
			}
		}
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
		//此方法废弃,请直接到KWForm内更改
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
				player->sendMsg("暂不支持售出 " + item->getName() + " * " + std::to_string(item->getStackSize()));
				return true;
			}
			else if (ret == 2) {
				player->sendMsg("您所售出的数量不足");
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
			player->sendMsg("你无法在此领地放置方块!");
			return false;
		}
		return true;
	}

	bool static BreakBlock(Player* player, const Block* block, BlockPos* blockpos) {


		if (!Land::canLandModifyBlock(Land::BlockChunckId(blockpos->getPosition()), player)) {
			player->sendMsg("你无法在此领地破坏方块!");
			return false;
		}

		if (block->getLegacyBlock()->getBlockItemID() == 54) {
			if (LockBox::isLockBox(blockpos->getPosition()->toNormalString())) {
				player->sendMsg("请先使用 /unlockbox 解锁此箱子");
				return false;
			}
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
			player->sendMsg("您已获得 " + std::to_string(aw));
			return false;
		}

		if (ChestShop::isChestShop(blockposition)) {
			return true;
		}
		if (!LockBox::HavePermission(player->getRealNameTag(), blockposition->getPosition()->toNormalString())) {
			player->sendMsg("您没有权限打开此箱子!");
			return false;
		}
		if (LockBox::isRequestLockBox(player->getRealNameTag())) {
			LockBox::SetPermission(player->getRealNameTag(), blockposition->getPosition()->toNormalString());
			player->sendMsg("您已成功上锁!");
			return false;
		}
		if (LockBox::isRequestUnLockBox(player->getRealNameTag())) {
			LockBox::RemoveLockBox(blockposition->getPosition()->toNormalString());
			player->sendMsg("您已成功解锁!");

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
		if (!Guild::isInGuild(player->getRealNameTag(), AdminGuild)) {
			SYMCALL(void, MSSYM_B1QA9setCanFlyB1AA5ActorB2AAA5QEAAXB1UA1NB1AA1Z, player, false); //设置玩家不可飞行
		}
		if (CConfig::GetValueString("Player", "Darkroom", player->getRealNameTag(), "false") == "true") {
			runcmd("tp " + player->getRealNameTag() + " " + CConfig::GetValueString("Settings", "Settings", "darkroom", "0 0 0"));
			player->sendMsg("你已被关在小黑屋反思.");
		}
	}


	bool static Move(Player* player) {
		//进入新领地
		string chunckid = Land::PlayerChunckId(player->getPos());
		string playername = player->getRealNameTag();
		if (chunckid != PlayerLastLandId[playername]) {
			//进入了新区块
			PlayerLastLandId[playername] = chunckid;
			if (Land::isLandOwned(chunckid)) {
				PlayerLastinLand[playername] = true;
				runcmd("title " + playername + u8" actionbar 您已进入 " + Land::getLandName(chunckid) + u8" §r领地公会: " + Land::getLandOwner(chunckid));
			}
			else {
				if (PlayerLastinLand[playername]) {
					runcmd("title " + playername + u8" actionbar 您已进入 野外, 此地暂未购买");
					PlayerLastinLand[playername] = false;
				}
			}
		}
		return true;
	}

	void static ReSpawn(Player* player) {
		if (CConfig::GetValueString("Player", "Darkroom", player->getRealNameTag(), "false") == "true") {
			runcmd("tp " + player->getRealNameTag() + " " + CConfig::GetValueString("Settings", "Settings", "darkroom", "0 0 0"));
			player->sendMsg("你已被关在小黑屋反思.");
		}
	}

	// 重设新名字
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
		if (Command::onPlayerSendCommand(command, player)) return false; //问为什么,要拦截下来他本来的Process
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
				player->sendMsg("你无法在此领地攻击他!");
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

#pragma once
#include "Prebuild.h" //Prebuild Header
#include "BDSAPI.hpp"
#include "CConfig.cpp"
#include "Economy.cpp"
#include <fstream>
#include <sstream>

class ChestShop {
public:
	int static SellItem(ItemStack* item, Player* player, int count) {
		int price = CConfig::GetValueInt("ChestShop", std::to_string(item->getId()), "sell", -1);
		if (price <= 0) {
			return -1;
		}
		else {
			if (count >= CConfig::GetValueInt("ChestShop", std::to_string(item->getId()), "count", -1)) {
				int rc = count / CConfig::GetValueInt("ChestShop", std::to_string(item->getId()), "count", 64);
				int rprice = rc * price;
				Economy::GivePlayerMoney(player->getRealNameTag(), rprice);
				player->sendMsg("成功售出 " + item->getName() + " * " + std::to_string(item->getStackSize()) + " = " + std::to_string(rprice));
				return 1;
			}
			else {
				return -2;
			}
		}
	}

	bool static isChestShop(BlockPos* bp) {
		return CConfig::GetValueInt("ChestShop", "Chest", bp->getPosition()->toNormalString(), 0) == 1;
	}

	void static setChestShop(Player* player, BlockPos* bp) {
		CConfig::SetValueString("ChestShop", "Request", player->getRealNameTag(), "false");
		CConfig::SetValueString("ChestShop", "Chest", bp->getPosition()->toNormalString(), "1");
	}

	void static RequestSetChestShop(Player* player) {
		CConfig::SetValueString("ChestShop", "Request", player->getRealNameTag(), "true");
	}

	bool static isRequestSetChestShop(Player* player) {
		return CConfig::GetValueString("ChestShop", "Request", player->getRealNameTag(), "false") == "true";
	}
	/* TODO
	void static sendBuyForm(Player* player) {
		std::ifstream in(CConfig::GetPluginPath() + "SellShop.json");
		std::ostringstream tmp;
		tmp << in.rdbuf();
		std::string str = tmp.str();
		UINT fid = sendForm((VA)player, (char*)str.c_str());
		CConfig::SetValueString("ChestShop", "SellForm", player->getRealNameTag(), std::to_string(fid));
	}
	*/
	bool static isRequestBuyForm(Player* player) {
		return CConfig::GetValueString("ChestShop", "SellForm", player->getRealNameTag(), "NaN") != "NaN";
	}

	bool static realbuy(Player* player, int select) {
		string id = CConfig::GetValueString("ChestShop", "SellList", std::to_string(select), "-1");
		if (id == "-1" || select == -1) return false;
		int price = CConfig::GetValueInt("ChestShop", id, "buy", 0);
		if (Economy::GetPlayerMoney(player->getRealNameTag()) < price) {
			player->sendMsg("您的余额不足以购买!");
			return false;
		}
		Economy::RemovePlayerMoney(player->getRealNameTag(),price);
		runcmd("give " + player->getRealNameTag() + " " + CConfig::GetValueString("ChestShop", id, "id", "NaN") + " " + CConfig::GetValueString("ChestShop", id, "count", "64"));
		return true;
	}
};
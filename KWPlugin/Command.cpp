#pragma once

#include "Prebuild.h" //Prebuild Header
#include "BDSAPI.hpp"
#include "Economy.cpp"
#include "LockBox.cpp"
#include "Guild.cpp"
#include "RPG.cpp"
#include "Land.cpp"
#include "AwardBox.cpp"
#include "ChestShop.cpp"
#include "KWForm.cpp"
#include "Shop.cpp"

static std::map<std::string, Player*> onlinePlayers;
static std::map<std::string, std::string> NametoUuid;

using namespace std;

class Command {
public:

	vector<string> static SplitStr(string str, char a)
	{
		vector<string> ret;
		size_t size = str.size();
		bool quoate = false;
		string temp = "";
		for (int i = 0; i <= size; i++) {
			if (str[i] == '"') {
				quoate = !quoate;
				temp += str[i];
				continue;
			}
			if (str[i] == a && quoate == false) {
				ret.push_back(temp);
				temp = "";
			}
			else {
				temp += str[i];
			}
		}
		temp = temp.substr(0, temp.length() - 1);
		ret.push_back(temp);
		return ret;
	}

	string static intToString(int v)
	{
		return std::to_string(v);
	}

	bool static onConsoleSendCommand(std::string cmd) {
		vector<string> params = SplitStr(cmd, ' ');
		return !ProcessConsoleCommand(params);
	}

	bool static onPlayerSendCommand(std::string cmd, Player* player) {
		vector<string> params = SplitStr(cmd, ' ');
		return ProcessCommand(params, player);
	}



	bool static ProcessCommand(vector<string> param, Player* player) {
		if (param[0] == "/money" && param.size() == 1) {
			player->sendMsg("你的余额为: §l§a" + intToString(Economy::GetPlayerMoney(player->getRealNameTag())));
		}
		else if (param[0] == "money") {
			if (param.size() == 2) {
				cout << "他的余额为: " << intToString(Economy::GetPlayerMoney(param[1])) << endl;
			}
			else {
				cout << "用法: /money <玩家名>" << endl;
			}
		}
		else if (param[0] == "/pay") {
			if (param.size() != 3) {
				player->sendMsg("用法: /pay <玩家名> <金额>");
				return true;
			}
			int willgive = atoi(param[2].c_str());
			if (Economy::GetPlayerMoney(player->getRealNameTag()) < willgive && !isAdmin(player)) {
				player->sendMsg("你的余额为: §l§c" + intToString(Economy::GetPlayerMoney(player->getRealNameTag())));
				player->sendMsg("不足以支付 §l§c" + param[2]);
			}
			else {
				if (willgive <= 0) {
					player->sendMsg("金额输入有误 " + param[2]);
					return true;
				}
				Economy::GivePlayerMoney(param[1], willgive);
				if (!isAdmin(player)) {
					Economy::RemovePlayerMoney(player->getRealNameTag(), willgive);
				}
				player->sendMsg("成功给 §a§l" + param[1] + " §r金额 §a§l" + param[2]);
				player->sendMsg("你的余额为: §l§a" + intToString(Economy::GetPlayerMoney(player->getRealNameTag())));
				string sendoutuuid = NametoUuid[param[1]];
				if (sendoutuuid != "") {//玩家在线
					onlinePlayers[sendoutuuid]->sendMsg("玩家: " + player->getRealNameTag() + " 给你转账 §l§a" + param[2]);
					onlinePlayers[sendoutuuid]->sendMsg("你当前余额为: §l§a" + intToString(Economy::GetPlayerMoney(onlinePlayers[sendoutuuid]->getRealNameTag())));
				}
			}
		}

		else if (param[0] == "/tpa") {
			if (param.size() == 1) {
				ButtonsForm form;
				form.SetTitle("TPA");
				form.SetText("请选择你要传送到的玩家:");
				map<string, string>::iterator iter;
				for (iter = NametoUuid.begin(); iter != NametoUuid.end(); iter++) {
					FormButton btn;
					FunctionButton funbtn;
					btn.haveimage = true;
					btn.imgpath = "textures/ui/icon_steve";
					btn.text = iter->first;
					funbtn.command = "/tpa " + iter->first;
					funbtn.type = 2;
					form.AddButton(btn,funbtn);
				}
				form.releaseForm((VA)player);
				return true;
			}
			if (param.size() != 2) {
				player->sendMsg("用法: /tpa <玩家名>");
				return true;
			}
			if (Economy::GetPlayerMoney(player->getRealNameTag()) >= Economy::GetPriceToDo("TPA")) {
				string sendoutuuid = NametoUuid[param[1]];
				if (sendoutuuid != "") {//玩家在线
					CConfig::SetValueString("TPA", param[1], "from", player->getRealNameTag());
					CConfig::SetValueString("TPA", param[1], "time", intToString(time(NULL)));
					onlinePlayers[sendoutuuid]->sendMsg("玩家: " + player->getRealNameTag() + " 请求tpa到你这里 §l§a");
					onlinePlayers[sendoutuuid]->sendMsg("输入 §l§a/tpayes §r即可同意请求");
					onlinePlayers[sendoutuuid]->sendMsg("输入 §l§c/tpano §r即可不同意请求");
					onlinePlayers[sendoutuuid]->sendMsg("该请求 §l§a60秒 §r后过期,到时候可以忽略");
					player->sendMsg("发送请求§l§a成功!§r在60秒内对方可以接受你的请求");
				}
				else {
					player->sendMsg("玩家: " + param[1] + " 不在线!");
					return true;
				}
			}
			else {
				player->sendMsg("你的余额不足以使用tpa");
			}
		}
		else if (param[0] == "/tpayes") {
			if (atoi(CConfig::GetValueString("TPA", player->getRealNameTag(), "time", "NaN").c_str()) < time(NULL) - 61)
			{
				player->sendMsg("§l§c所有请求已过期!");
				return true;
			}
			string from = CConfig::GetValueString("TPA", player->getRealNameTag(), "from", "");
			string sendoutuuid = NametoUuid[from];
			if (sendoutuuid != "") {//玩家在线
				CConfig::SetValueString("TPA", player->getRealNameTag(), "time", intToString(0));
				runcmd("tp " + from + " " + player->getRealNameTag());
				Economy::RemovePlayerMoney(player->getRealNameTag(), Economy::GetPriceToDo("TPA"));
				onlinePlayers[sendoutuuid]->sendMsg("成功TPA到" + player->getRealNameTag());
				player->sendMsg("玩家 " + from + " 已成功TPA到此处");
			}
			else {
				player->sendMsg("§l§c请求发起者已下线!");
			}
		}
		else if (param[0] == "/tpano") {
			string from = CConfig::GetValueString("TPA", player->getRealNameTag(), "from", "");
			string sendoutuuid = NametoUuid[from];
			if (sendoutuuid != "") {//玩家在线
				CConfig::SetValueString("TPA", player->getRealNameTag(), "time", intToString(0));
				onlinePlayers[sendoutuuid]->sendMsg("玩家: " + player->getRealNameTag() + " 拒绝了你的TPA请求");
				player->sendMsg("成功拒绝 " + from + " 的TPA请求");
			}
			else {
				player->sendMsg("§l§c请求发起者已下线!");
			}
		}

		else if (param[0] == "/lockbox") {
			if (Economy::GetPlayerMoney(player->getRealNameTag()) >= Economy::GetPriceToDo("LockBox")) {
				Economy::RemovePlayerMoney(player->getRealNameTag(), Economy::GetPriceToDo("LockBox"));
				LockBox::RequestLockBox(player->getRealNameTag());
				player->sendMsg("请点击要锁的箱子!");
			}
			else {
				player->sendMsg("你的余额不足!");
			}
		}
		else if (param[0] == "/unlockbox") {
			LockBox::RequestUnLockBox(player->getRealNameTag());
			player->sendMsg("请点击要解锁的箱子!");
		}
		else if (param[0] == "/g") {
			if (param[1] == "join") {
				if (Guild::isInGuild(player->getRealNameTag())) {
					player->sendMsg("你当前已在公会中, 输入 /g exit 来退出当前公会");
					return true;
				}
				if (param.size() == 3) {
					if (Guild::isGuildSet(param[2])) {
						Guild::RequestJoin(param[2], player->getRealNameTag());
						string sendoutuuid = NametoUuid[Guild::GetAdmin(param[2])];
						if (sendoutuuid != "") {//玩家在线						
							onlinePlayers[sendoutuuid]->sendMsg("玩家: " + player->getRealNameTag() + " 请求加入公会 " + param[2]);
							onlinePlayers[sendoutuuid]->sendMsg("输入 §l§a/g accept §r即可同意请求");
							player->sendMsg("成功发送请求给公会管理员!");
							return true;

						}
						else {
							player->sendMsg("§l§c公会管理员已下线! 在线后可通过 §l§a/g accept §r同意请求");
							return true;

						}
					}
					else {
						player->sendMsg("公会不存在!");
					}
				}
				else {
					player->sendMsg("用法: /g join <公会名>");
				}
			}
			if (param[1] == "accept") {
				Guild::AccecptJoin(Guild::PlayerInWhich(player->getRealNameTag()), player);
			}
			if (param[1] == "create") {
				if (Guild::isInGuild(player->getRealNameTag())) {
					player->sendMsg("你已经在公会了!");
					player->sendMsg("可以输入 /g exit 来退出当前公会");
				}
				else {
					if (param.size() == 3) {
						if (!Guild::isGuildSet(param[2])) {
							if (Economy::GetPlayerMoney(player->getRealNameTag()) >= Economy::GetPriceToDo("CreateGuild")) {
								Guild::CreateGuild(param[2], player->getRealNameTag());
								Economy::RemovePlayerMoney(player->getRealNameTag(), Economy::GetPriceToDo("CreateGuild"));
								player->sendMsg("成功创建公会!");
							}
							else {
								player->sendMsg("你的余额不足!");
							}
						}
						else
						{
							player->sendMsg("该公会已成立");
						}
					}
					else {
						player->sendMsg("用法: /g create <公会名> ");
					}
				}
			}
			if (param[1] == "exit") {
				if (Guild::isAdmin(player->getRealNameTag(), Guild::PlayerInWhich(player->getRealNameTag()))) {
					Guild::RemoveGuild(Guild::PlayerInWhich(player->getRealNameTag()), player);
				}
				Guild::ExitGuild(Guild::PlayerInWhich(player->getRealNameTag()), player);
			}
			if (param[1] == "member") {
				if (Guild::isInGuild(player->getRealNameTag())) {
					string guild = Guild::PlayerInWhich(player->getRealNameTag());
					player->sendMsg(guild + " 成员有:");
					player->sendMsg(Guild::GetPlayers(guild));
				}
				else {
					player->sendMsg("你还未加入或创建公会");
				}
			}
		}
		else if (param[0] == "/chunck" && isAdmin(player)) {
			player->sendMsg("当前在 " + Land::PlayerChunckId(player->getPos()));
		}
		else if (param[0] == "/l") {
			if (param[1] == "buy") {
				if (Economy::GetPlayerMoney(player->getRealNameTag()) >= Economy::GetPriceToDo("BuyLand")) {
					if (Land::isLandOwned(Land::PlayerChunckId(player->getPos()))) {
						player->sendMsg("领地已被购买! 你可以输入 §a/l info§r 查看详细信息");
					}
					else {
						if (Guild::isInGuild(player->getNameTag())) {
							Land::GiveLand(Land::PlayerChunckId(player->getPos()), Guild::getPlayerGuildName(player->getRealNameTag()));
							Economy::RemovePlayerMoney(player->getRealNameTag(), Economy::GetPriceToDo("BuyLand"));
							player->sendMsg("您已成功购买领地! 领地编号" + Land::PlayerChunckId(player->getPos()));
						}
						else {
							player->sendMsg("你还未加入公会,请先创建或加入一个公会");
						}
					}
				}
				else {
					player->sendMsg("你的余额不足以购买!");
				}
			}
			if (param[1] == "info") {
				if (!Land::isLandOwned(Land::PlayerChunckId(player->getPos()))) {
					player->sendMsg("当前领地暂未被购买 输入 §a/l buy§r 购买此领地");
				}
				else {
					string chuck = Land::PlayerChunckId(player->getPos());
					player->sendMsg("领地编号: " + chuck);
					player->sendMsg("领地名称: " + Land::getLandName(chuck));
					player->sendMsg("领地公会: " + Land::getLandOwner(chuck));
				}
			}
			if (param[1] == "name") {
				string chunck = Land::PlayerChunckId(player->getPos());

				if (!Land::isLandOwned(chunck)) {
					player->sendMsg("当前领地暂未被购买 输入 §a/l buy§r 购买此领地");
				}
				else {
					if (param.size() == 3) {

						if (Guild::isInGuild(player->getRealNameTag(), Land::getLandOwner(chunck))) {
							Land::setLandName(chunck, param[2]);
							player->sendMsg("成功设置");

						}
						else {
							player->sendMsg("你不是此领地成员");
						}
					}
					else {
						player->sendMsg("用法 /l name <领地名称>");
					}
				}
			}
		}
		else if (param[0] == "/ab" && isAdmin(player)) {
			if (param[1] != "tp") {
				AwardBox::RequestSet(player, param[1], param[2] == "once" ? true : false);
			}
			else {
				AwardBox::RequestTP(player);
			}
		}
		else if (param[0] == "/cs" && isAdmin(player)) {
			ChestShop::RequestSetChestShop(player);
			player->sendMsg("请点击ChestShop");
		}
		else if (param[0] == "/buy") {
			if (param.size() != 2) {
				player->sendMsg("参数填写错误!");
			}
			else {
				ShopItem item = ShopItem(param[1]);
				cout << "The data id is: " << item.dataid << endl;
				if (Economy::GetPlayerMoney(player->getRealNameTag()) < item.buyprice) {
					player->sendMsg("你的余额不足以购买 " + UTF8ToGBK(item.name.c_str()) + " * " + std::to_string(item.cont) + " = " + std::to_string(item.buyprice));
				}
				else {
					Economy::RemovePlayerMoney(player->getRealNameTag(), item.buyprice);
					player->addItem(item.nameid, std::to_string(item.cont), std::to_string(item.dataid));
					player->sendMsg("您已成功购买 " + UTF8ToGBK(item.name.c_str()) + " * " + std::to_string(item.cont) + " = " + std::to_string(item.buyprice));
				}
			}
		}
		else if (param[0] == "/buyshop") {
			vector<ShopItem> items = Shop::GetShopSellItem();
			ButtonsForm form = ButtonsForm();
			form.SetTitle("购买商店");
			form.SetText("欢迎来到购买商店! 你可以在这里买到你想要的东西!");
			for (int i = 0; i < items.size(); i++)
			{
				ShopItem item = items[i];
				FormButton fb;
				FunctionButton	funb;
				fb.text = item.name + " * " + std::to_string(item.cont) + " = " + std::to_string(item.buyprice);
				fb.haveimage = true;
				fb.isimgurl = false;
				fb.imgpath = item.texture;
				funb.command = "/buy " + item.uniqueid;
				funb.type = 2;
				form.AddButton(fb, funb);
			}
			form.releaseForm((VA)player);
		}
		else if (param[0] == "/sellshop") {
			vector<ShopItem> items = Shop::GetShopSellItem();
			ButtonsForm form = ButtonsForm();
			form.SetTitle("回收商店");
			form.SetText("欢迎来到回收商店! 你可以在这里将物品兑换成货币!");
			for (int i = 0; i < items.size(); i++)
			{
				ShopItem item = items[i];
				FormButton fb;
				FunctionButton	funb;
				fb.text = item.name + " * " + std::to_string(item.cont) + " = " + std::to_string(item.sellprice);
				fb.haveimage = true;
				fb.isimgurl = false;
				fb.imgpath = item.texture;
				funb.command = "/sell " + item.uniqueid;
				funb.type = 2;
				form.AddButton(fb, funb);
			}
			form.releaseForm((VA)player);

		}
		else if (param[0] == "/sell") {
			if (param.size() != 2) {
				player->sendMsg("参数填写错误!");
			}
			else {
				ItemStack* onhand = player->getSelectedItem();
				ShopItem shopitem = ShopItem(onhand->getId(), onhand->getAuxValue());
				if (shopitem.uniqueid == param[1]) {
					if (onhand->getStackSize() >= shopitem.cont) {
						Economy::GivePlayerMoney(player->getRealNameTag(), shopitem.sellprice);
						runcmd("clear " + player->getRealNameTag() + " " + shopitem.nameid + " " + std::to_string(shopitem.dataid) + " " + std::to_string(shopitem.cont));
						player->sendMsg("成功回收 " + shopitem.name + " * " + std::to_string(shopitem.cont) + " = " + std::to_string(shopitem.sellprice));
					}
					else {
						player->sendMsg("回收的物品数量不够!");

					}
				}
				else {
					player->sendMsg("请先将要回收的物品放在手上再进行回收!");
				}
			}
		}
		else if (param[0] == "/maincity") {
			runcmd("tp " + player->getRealNameTag() + " " + CConfig::GetValueString("Settings", "Settings", "maincity"));
			player->sendMsg("您已成功回城");
		}
		else if (param[0] == "/setmaincity" && isAdmin(player)) {
			CConfig::SetValueString("Settings", "Settings", "maincity", player->getPos()->toNormalString());
			player->sendMsg("成功设置主城!");
		}
		else if (param[0] == "/setdarkroom" && isAdmin(player)) {
			CConfig::SetValueString("Settings", "Settings", "darkroom", player->getPos()->toNormalString());
			player->sendMsg("成功设置小黑屋!");
		}
		else if (param[0] == "/darkroom" && isAdmin(player)) {
			runcmd("tp " + param[1] + " " + CConfig::GetValueString("Settings", "Settings", "darkroom", "0 0 0"));
			CConfig::SetValueString("Player", "Darkroom", param[1], "true");
			string sendoutuuid = NametoUuid[param[1]];
			if (sendoutuuid != "") {//玩家在线	
				onlinePlayers[sendoutuuid]->sendMsg("你被关到了小黑屋");
			}
			player->sendMsg("成功让他到了小黑屋");
		}
		else if (param[0] == "/openroom" && isAdmin(player)) {
			runcmd("tp " + param[1] + " " + CConfig::GetValueString("Settings", "Settings", "maincity"));
			CConfig::SetValueString("Player", "Darkroom", param[1], "false");
			string sendoutuuid = NametoUuid[Guild::GetAdmin(param[1])];
			if (sendoutuuid != "") {//玩家在线	
				onlinePlayers[sendoutuuid]->sendMsg("管理员把你放了出来");
			}
			player->sendMsg("成功让他出来了");
		}
		else if (param[0] == "/home") {
			if (Economy::GetPlayerMoney(player->getRealNameTag()) >= Economy::GetPriceToDo("TPHome")) {
				player->sendMsg("你的余额不足以执行此操作");
			}
			else {
				string home = CConfig::GetValueString("Player", player->getRealNameTag(), "home", "NaN");
				if (home == "NaN") {
					player->sendMsg("你还没有设置Home");
				}
				else {
					runcmd("tp " + player->getRealNameTag() + " " + home);
					Economy::RemovePlayerMoney(player->getRealNameTag(), 1);
				}
			}
		}
		else if (param[0] == "/sethome") {
			if (Economy::GetPlayerMoney(player->getRealNameTag()) >= Economy::GetPriceToDo("SetHome")) {
				player->sendMsg("你的余额不足以执行此操作");
			}
			else {
				CConfig::SetValueString("Player", player->getRealNameTag(), "home", player->getPos()->toNormalString());
				Economy::RemovePlayerMoney(player->getRealNameTag(), Economy::GetPriceToDo("SetHome"));
				player->sendMsg("成功设置Home!");
			}
		}
		/*TODO
		else if (param[0] == "/ranklist") {
			CConfig::GetSections("Economy");
		}
		*/
		else if (param[0] == "/qd") {
			Economy::DailySign(player);
		}
		else if (param[0] == "/setprice" && isAdmin(player)) {
			Economy::SetPriceToDo(param[1], param[2]);
		}
		else if (param[0] == "/setshop" && isAdmin(player)) {
			//   /setshop <买入价格> <售出价格> <起卖数量>
			ItemStack* item = player->getSelectedItem();
			if (item->isNull()) {
				player->sendMsg("请先选中一个东西! ");
				return true;
			}
			ShopItem shopitem = ShopItem(item->getId(), item->getAuxValue());
			shopitem.buyprice = atoi(param[1].c_str());
			shopitem.sellprice = atoi(param[2].c_str());
			shopitem.cont = atoi(param[3].c_str());
			shopitem.name = item->getName();
			shopitem.nameid = item->getRawNameId();
			shopitem.texture = "textures/items/" + item->getRawNameId();
			shopitem.SaveItem();
			player->sendMsg("商品设置成功!");
		}
		else {
			return false;
		}
		return true;
	}

	bool static isAdmin(Player* player) {
		return Guild::isInGuild(player->getRealNameTag(), AdminGuild);
	}

	bool static ProcessConsoleCommand(vector<string> param) {
		if (param[0] == "money") {
			if (param.size() == 2) {
				cout << "他的余额为: " << intToString(Economy::GetPlayerMoney(param[1])) << endl;
			}
			else {
				cout << "用法: money <玩家名>" << endl;
			}
		}
		else if (param[0] == "pay") {
			if (param.size() == 3) {
				cout << "给钱成功! 他的余额目前为: " << Economy::GivePlayerMoney(param[1], atoi(param[2].c_str())) << endl;
				string sendoutuuid = NametoUuid[param[1]];
				if (sendoutuuid != "") {//玩家在线
					onlinePlayers[sendoutuuid]->sendMsg("管理员 给你转账 §l§a" + param[2]);
					onlinePlayers[sendoutuuid]->sendMsg("你当前余额为: §l§a" + intToString(Economy::GetPlayerMoney(onlinePlayers[sendoutuuid]->getRealNameTag())));
				}
			}
			else {
				cout << "用法: pay <玩家名> <数量>" << endl;
			}
		}
		else if (param[0] == "setadminguild") {
			CConfig::SetValueString("Settings", "Settings", "AdminGuild", param[1]);
			AdminGuild = param[1];
			cout << "Admin Guild Now Is: " << AdminGuild << endl;
		}
		else if (param[0] == "darkroom") {
			runcmd("tp " + param[1] + " " + CConfig::GetValueString("Settings", "Settings", "darkroom", "0 0 0"));
			CConfig::SetValueString("Player", "Darkroom", param[1], "true");
			string sendoutuuid = NametoUuid[Guild::GetAdmin(param[1])];
			if (sendoutuuid != "") {//玩家在线	
				onlinePlayers[sendoutuuid]->sendMsg("你被关到了小黑屋");
			}
			cout << "成功让他到了小黑屋" << endl;
		}
		else if (param[0] == "setprice") {
			Economy::SetPriceToDo(param[1], param[2]);
		}
		else if (param[0] == "openroom") {
			runcmd("tp " + param[1] + " " + CConfig::GetValueString("Settings", "Settings", "maincity"));
			CConfig::SetValueString("Player", "Darkroom", param[1], "false");
			string sendoutuuid = NametoUuid[Guild::GetAdmin(param[1])];
			if (sendoutuuid != "") {//玩家在线	
				onlinePlayers[sendoutuuid]->sendMsg("管理员把你放了出来");
			}
			cout << "成功让他出来了" << endl;
		}
		else {
			return false;
		}
		return true;
	}

};


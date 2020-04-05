#pragma once
#include <string>
#include <vector>
#include <algorithm>// std::replace_if
#include "BDSAPI.hpp"
#include "Economy.cpp"
#include "LockBox.cpp"
#include <time.h>
#include "Guild.cpp"
#include "RPG.cpp"
#include "Land.cpp"
#include "AwardBox.cpp"
#include "ChestShop.cpp"

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
			player->sendMsg("������Ϊ: ��l��a" + intToString(Economy::GetPlayerMoney(player->getRealNameTag())));
		}
		else if (param[0] == "money") {
			if (param.size() == 2) {
				cout << u8"�������Ϊ: " << intToString(Economy::GetPlayerMoney(param[1])) << endl;
			}
			else {
				cout << u8"�÷�: /money <�����>" << endl;
			}
		}
		else if (param[0] == "/pay") {
			if (param.size() != 3) {
				player->sendMsg("�÷�: /pay <�����> <���>");
				return true;
			}
			int willgive = atoi(param[2].c_str());
			if (Economy::GetPlayerMoney(player->getRealNameTag()) < willgive && !isAdmin(player)) {
				player->sendMsg("������Ϊ: ��l��c" + intToString(Economy::GetPlayerMoney(player->getRealNameTag())));
				player->sendMsg("������֧�� ��l��c" + param[2]);
			}
			else {
				if (willgive <= 0) {
					player->sendMsg("����������� " + param[2]);
					return true;
				}
				Economy::GivePlayerMoney(param[1], willgive);
				Economy::RemovePlayerMoney(player->getRealNameTag(), willgive);
				player->sendMsg("�ɹ��� ��a��l" + param[1] + " ��r��� ��a��l" + param[2]);
				player->sendMsg("������Ϊ: ��l��a" + intToString(Economy::GetPlayerMoney(player->getRealNameTag())));
				string sendoutuuid = NametoUuid[param[1]];
				if (sendoutuuid != "") {//�������
					onlinePlayers[sendoutuuid]->sendMsg("���: " + player->getRealNameTag() + " ����ת�� ��l��a" + param[2]);
					onlinePlayers[sendoutuuid]->sendMsg("�㵱ǰ���Ϊ: ��l��a" + intToString(Economy::GetPlayerMoney(onlinePlayers[sendoutuuid]->getRealNameTag())));
				}
			}
		}
		else if (param[0] == "/tpa") {
			if (param.size() != 2) {
				player->sendMsg("�÷�: /tpa <�����>");
				return true;
			}
			if (Economy::GetPlayerMoney(player->getRealNameTag()) >= Economy::GetPriceToDo("TPA")) {
				string sendoutuuid = NametoUuid[param[1]];
				if (sendoutuuid != "") {//�������
					CConfig::SetValueString("TPA", param[1], "from", player->getRealNameTag());
					CConfig::SetValueString("TPA", param[1], "time", intToString(time(NULL)));
					onlinePlayers[sendoutuuid]->sendMsg("���: " + player->getRealNameTag() + " ����tpa�������� ��l��a");
					onlinePlayers[sendoutuuid]->sendMsg("���� ��l��a/tpayes ��r����ͬ������");
					onlinePlayers[sendoutuuid]->sendMsg("���� ��l��c/tpano ��r���ɲ�ͬ������");
					onlinePlayers[sendoutuuid]->sendMsg("������ ��l��a60�� ��r�����,��ʱ����Ժ���");
					player->sendMsg("���������l��a�ɹ�!��r��60���ڶԷ����Խ����������");
				}
				else {
					player->sendMsg("���: " + param[1] + " ������!");
					return true;
				}
			}
			else {
				player->sendMsg("���������ʹ��tpa");
			}
		}
		else if (param[0] == "/tpayes") {
			if (atoi(CConfig::GetValueString("TPA", player->getRealNameTag(), "time", "NaN").c_str()) < time(NULL) - 61)
			{
				player->sendMsg("��l��c���������ѹ���!");
				return true;
			}
			string from = CConfig::GetValueString("TPA", player->getRealNameTag(), "from", "");
			string sendoutuuid = NametoUuid[from];
			if (sendoutuuid != "") {//�������
				CConfig::SetValueString("TPA", player->getRealNameTag(), "time", intToString(0));
				runcmd("tp " + from + " " + player->getRealNameTag());
				Economy::RemovePlayerMoney(player->getRealNameTag(), Economy::GetPriceToDo("TPA"));
				onlinePlayers[sendoutuuid]->sendMsg("�ɹ�TPA��" + player->getRealNameTag());
				player->sendMsg("��� " + from + " �ѳɹ�TPA���˴�");
			}
			else {
				player->sendMsg("��l��c��������������!");
			}
		}
		else if (param[0] == "/tpano") {
			string from = CConfig::GetValueString("TPA", player->getRealNameTag(), "from", "");
			string sendoutuuid = NametoUuid[from];
			if (sendoutuuid != "") {//�������
				CConfig::SetValueString("TPA", player->getRealNameTag(), "time", intToString(0));
				onlinePlayers[sendoutuuid]->sendMsg("���: " + player->getRealNameTag() + " �ܾ������TPA����");
				player->sendMsg("�ɹ��ܾ� " + from + " ��TPA����");
			}
			else {
				player->sendMsg("��l��c��������������!");
			}
		}
		else if (param[0] == "/lock") {
			if (Economy::GetPlayerMoney(player->getRealNameTag()) >= Economy::GetPriceToDo("LockBox")) {
				Economy::RemovePlayerMoney(player->getRealNameTag(), Economy::GetPriceToDo("LockBox"));
				LockBox::RequestLockBox(player);
				player->sendMsg("����Ҫ��������!");
			}
			else {
				player->sendMsg("�������!");
			}
		}
		else if (param[0] == "/g") {
			if (param[1] == "join") {
				if (Guild::isInGuild(player->getRealNameTag())) {
					player->sendMsg("�㵱ǰ���ڹ�����, ���� /g exit ���˳���ǰ����");
					return true;
				}
				if (param.size() == 3) {
					if (Guild::isGuildSet(param[2])) {
						Guild::RequestJoin(param[2], player->getRealNameTag());
						string sendoutuuid = NametoUuid[Guild::GetAdmin(param[2])];
						if (sendoutuuid != "") {//�������						
							onlinePlayers[sendoutuuid]->sendMsg("���: " + player->getRealNameTag() + " ������빫�� " + param[2]);
							onlinePlayers[sendoutuuid]->sendMsg("���� ��l��a/g accept ��r����ͬ������");
							player->sendMsg("�ɹ�����������������Ա!");
							return true;

						}
						else {
							player->sendMsg("��l��c�������Ա������! ���ߺ��ͨ�� ��l��a/g accept ��rͬ������");
							return true;

						}
					}
					else {
						player->sendMsg("���᲻����!");
					}
				}
				else {
					player->sendMsg("�÷�: /g join <������>");
				}
			}
			if (param[1] == "accept") {
				Guild::AccecptJoin(Guild::PlayerInWhich(player->getRealNameTag()), player);
			}
			if (param[1] == "create") {
				if (Guild::isInGuild(player->getRealNameTag())) {
					player->sendMsg("���Ѿ��ڹ�����!");
					player->sendMsg("�������� /g exit ���˳���ǰ����");
				}
				else {
					if (param.size() == 3) {
						if (!Guild::isGuildSet(param[2])) {
							player->sendMsg(param[2] + " ������");
							if (Economy::GetPlayerMoney(player->getRealNameTag()) >= Economy::GetPriceToDo("CreateGuild")) {
								Guild::CreateGuild(param[2], player->getRealNameTag());
								Economy::RemovePlayerMoney(player->getRealNameTag(), Economy::GetPriceToDo("CreateGuild"));
								player->sendMsg("�ɹ���������!");
							}
							else {
								player->sendMsg("�������!");
							}
						}
						else
						{
							player->sendMsg("�ù����ѳ���");
						}
					}
					else {
						player->sendMsg("�÷�: /g create <������> ");
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
					player->sendMsg(guild + " ��Ա��:");
					player->sendMsg(Guild::GetPlayers(guild));
				}
				else {
					player->sendMsg("�㻹δ����򴴽�����");
				}
			}
		}
		else if (param[0] == "/chunck" && isAdmin(player)) {
			player->sendMsg("��ǰ�� " + Land::PlayerChunckId(player->getPos()));
		}
		else if (param[0] == "/l") {
			if (param[1] == "buy") {
				if (Economy::GetPlayerMoney(player->getRealNameTag()) >= Economy::GetPriceToDo("BuyLand")) {
					if (Land::isLandOwned(Land::PlayerChunckId(player->getPos()))) {
						player->sendMsg("����ѱ�����! ��������� ��a/l info��r �鿴��ϸ��Ϣ");
					}
					else {
						if (Guild::isInGuild(player->getNameTag())) {
							Land::GiveLand(Land::PlayerChunckId(player->getPos()), Guild::getPlayerGuildName(player->getRealNameTag()));
							Economy::RemovePlayerMoney(player->getRealNameTag(), Economy::GetPriceToDo("BuyLand"));
							player->sendMsg("���ѳɹ��������! ��ر��" + Land::PlayerChunckId(player->getPos()));
						}
						else {
							player->sendMsg("�㻹δ���빫��,���ȴ��������һ������");
						}
					}
				}
				else {
					player->sendMsg("��������Թ���!");
				}
			}
			if (param[1] == "info") {
				if (!Land::isLandOwned(Land::PlayerChunckId(player->getPos()))) {
					player->sendMsg("��ǰ�����δ������ ���� ��a/l buy��r ��������");
				}
				else {
					string chuck = Land::PlayerChunckId(player->getPos());
					player->sendMsg("��ر��: " + chuck);
					player->sendMsg("��ع���: " + Land::getLandOwner(chuck));
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
			player->sendMsg("����ChestShop");
		}
		else if (param[0] == "/sd") {
			ChestShop::sendBuyForm(player);
		}
		else if (param[0] == "/maincity") {
			runcmd("tp " + player->getRealNameTag() + " 1495 67 54");
			player->sendMsg("���ѳɹ��س�");
		}
		else if (param[0] == "/darkroom" && isAdmin(player)) {
			runcmd("tp " + param[1] + " 1484 95 45");
			CConfig::SetValueString("Player", "Darkroom", param[1], "true");
			string sendoutuuid = NametoUuid[Guild::GetAdmin(param[1])];
			if (sendoutuuid != "") {//�������	
				onlinePlayers[sendoutuuid]->sendMsg("�㱻�ص���С����");
			}
			player->sendMsg("�ɹ���������С����");
		}
		else if (param[0] == "/openroom" && isAdmin(player)) {
			runcmd("tp " + param[1] + " 1495 67 54");
			CConfig::SetValueString("Player", "Darkroom", param[1], "false");
			string sendoutuuid = NametoUuid[Guild::GetAdmin(param[1])];
			if (sendoutuuid != "") {//�������	
				onlinePlayers[sendoutuuid]->sendMsg("����Ա������˳���");
			}
			player->sendMsg("�ɹ�����������");
		}
		else if (param[0] == "/home") {
			if (Economy::GetPlayerMoney(player->getRealNameTag()) >= Economy::GetPriceToDo("TPHome")) {
				player->sendMsg("���������ִ�д˲���");
			}
			else {
				string home = CConfig::GetValueString("Player", player->getRealNameTag(), "home", "NaN");
				if (home == "NaN") {
					player->sendMsg("�㻹û������Home");
				}
				else {
					runcmd("tp " + player->getRealNameTag() + " " + home);
					Economy::RemovePlayerMoney(player->getRealNameTag(), 1);
				}
			}
		}
		else if (param[0] == "/sethome") {
			if (Economy::GetPlayerMoney(player->getRealNameTag()) >= Economy::GetPriceToDo("SetHome")) {
				player->sendMsg("���������ִ�д˲���");
			}
			else {
				CConfig::SetValueString("Player", player->getRealNameTag(), "home", player->getPos()->toNormalString());
				Economy::RemovePlayerMoney(player->getRealNameTag(), Economy::GetPriceToDo("SetHome"));
				player->sendMsg("�ɹ�����Home!");
			}
		}
		else if (param[0] == "/qd") {
			Economy::DailySign(player);
		}
		else if (param[0] == "/setprice" && isAdmin(player)) {
			Economy::SetPriceToDo(param[1], param[2]);
		}
		else {
			return false;
		}
		return true;
	}

	bool static isAdmin(Player* player) {
		return Guild::isInGuild(player->getRealNameTag(), u8"TIC");
	}

	bool static ProcessConsoleCommand(vector<string> param) {
		if (param[0] == "money") {
			if (param.size() == 2) {
				cout << u8"�������Ϊ: " << intToString(Economy::GetPlayerMoney(param[1])) << endl;
			}
			else {
				cout << u8"�÷�: money <�����>" << endl;
			}
		}
		else if (param[0] == "pay") {
			if (param.size() == 3) {
				cout << u8"��Ǯ�ɹ�! �������ĿǰΪ: " << Economy::GivePlayerMoney(param[1], atoi(param[2].c_str())) << endl;
				string sendoutuuid = NametoUuid[param[1]];
				if (sendoutuuid != "") {//�������
					onlinePlayers[sendoutuuid]->sendMsg("����Ա ����ת�� ��l��a" + param[2]);
					onlinePlayers[sendoutuuid]->sendMsg("�㵱ǰ���Ϊ: ��l��a" + intToString(Economy::GetPlayerMoney(onlinePlayers[sendoutuuid]->getRealNameTag())));
				}
			}
			else {
				cout << u8"�÷�: pay <�����> <����>" << endl;
			}
		}
		else if (param[0] == "darkroom") {
			runcmd("tp " + param[1] + " 1484 95 45");
			CConfig::SetValueString("Player", "Darkroom", param[1], "true");
			string sendoutuuid = NametoUuid[Guild::GetAdmin(param[1])];
			if (sendoutuuid != "") {//�������	
				onlinePlayers[sendoutuuid]->sendMsg("�㱻�ص���С����");
			}
			cout << u8"�ɹ���������С����" << endl;
		}
		else if (param[0] == "openroom") {
			runcmd("tp " + param[1] + " 1495 67 54");
			CConfig::SetValueString("Player", "Darkroom", param[1], "false");
			string sendoutuuid = NametoUuid[Guild::GetAdmin(param[1])];
			if (sendoutuuid != "") {//�������	
				onlinePlayers[sendoutuuid]->sendMsg("����Ա������˳���");
			}
			cout << u8"�ɹ�����������" << endl;
		}
		else if (param[0] == "cleanmob") {
			runcmd("kill @e[type=item]");
			runcmd("me �����������l��a���");
		}
		else {
			return false;
		}
		return true;
	}

};
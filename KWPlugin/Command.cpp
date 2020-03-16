#pragma once
#include "Prebuild.h"
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

static std::map<std::string, Player*> onlinePlayers;
static std::map<std::string, std::string> NametoUuid;
using namespace std;

class Command {
public:

	vector<string> static SplitStr(string strtem, char a)
	{
		vector<string> strvec;

		string::size_type pos1, pos2;
		pos2 = strtem.find(a);
		pos1 = 0;
		while (string::npos != pos2)
		{
			strvec.push_back(strtem.substr(pos1, pos2 - pos1));

			pos1 = pos2 + 1;
			pos2 = strtem.find(a, pos1);
		}
		strvec.push_back(strtem.substr(pos1));
		return strvec;
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
		if (param[0] == "/money") {
			player->sendMsg("������Ϊ: ��l��a" + intToString(Economy::GetPlayerMoney(player->getNameTag())));
		}
		else if (param[0] == "/pay") {
			if (param.size() != 3) {
				player->sendMsg("�÷�: /pay <�����> <���>");
				return true;
			}
			int willgive = atoi(param[2].c_str());
			if (Economy::GetPlayerMoney(player->getNameTag()) < willgive) {
				player->sendMsg("������Ϊ: ��l��c" + intToString(Economy::GetPlayerMoney(player->getNameTag())));
				player->sendMsg("������֧�� ��l��c" + param[2]);
			}
			else {
				if (willgive <= 0) {
					player->sendMsg("����������� " + param[2]);
					return true;
				}
				Economy::GivePlayerMoney(param[1], willgive);
				Economy::RemovePlayerMoney(player->getNameTag(), willgive);
				player->sendMsg("�ɹ��� ��a��l" + param[1] + " ��r��� ��a��l" + param[2]);
				player->sendMsg("������Ϊ: ��l��a" + intToString(Economy::GetPlayerMoney(player->getNameTag())));
				string sendoutuuid = NametoUuid[param[1]];
				if (sendoutuuid != "") {//�������
					onlinePlayers[sendoutuuid]->sendMsg("���: " + player->getNameTag() + " ����ת�� ��l��a" + param[2]);
					onlinePlayers[sendoutuuid]->sendMsg("�㵱ǰ���Ϊ: ��l��a" + intToString(Economy::GetPlayerMoney(onlinePlayers[sendoutuuid]->getNameTag())));
				}
			}
		}
		else if (param[0] == "/tpa") {
			if (param.size() != 2) {
				player->sendMsg("�÷�: /tpa <�����>");
				return true;
			}
			if (Economy::GetPlayerMoney(player->getNameTag()) >= 1) {
				string sendoutuuid = NametoUuid[param[1]];
				if (sendoutuuid != "") {//�������
					CConfig::SetValueString("TPA", param[1], "from", player->getNameTag());
					CConfig::SetValueString("TPA", param[1], "time", intToString(time(NULL)));
					onlinePlayers[sendoutuuid]->sendMsg("���: " + player->getNameTag() + " ����tpa�������� ��l��a");
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
				Economy::GivePlayerMoney(player->getNameTag(), 1);
				player->sendMsg("���������ʹ��tpa");
			}

		}
		else if (param[0] == "/tpayes") {
			if (atoi(CConfig::GetValueString("TPA", player->getNameTag(), "time", "NaN").c_str()) < time(NULL) - 61)
			{
				player->sendMsg("��l��c���������ѹ���!");
				return true;
			}
			string from = CConfig::GetValueString("TPA", player->getNameTag(), "from", "");
			string sendoutuuid = NametoUuid[from];
			if (sendoutuuid != "") {//�������
				CConfig::SetValueString("TPA", player->getNameTag(), "time", intToString(0));
				runcmd("tp " + from + " " + player->getNameTag());
				onlinePlayers[sendoutuuid]->sendMsg("�ɹ�TPA��" + player->getNameTag());
				player->sendMsg("��� " + from + " �ѳɹ�TPA���˴�");
			}
			else {
				player->sendMsg("��l��c��������������!");
			}
		}
		else if (param[0] == "/tpano") {
			string from = CConfig::GetValueString("TPA", player->getNameTag(), "from", "");
			string sendoutuuid = NametoUuid[from];
			if (sendoutuuid != "") {//�������
				CConfig::SetValueString("TPA", player->getNameTag(), "time", intToString(0));
				onlinePlayers[sendoutuuid]->sendMsg("���: " + player->getNameTag() + " �ܾ������TPA����");
				player->sendMsg("�ɹ��ܾ� " + from + " ��TPA����");
			}
			else {
				player->sendMsg("��l��c��������������!");
			}
		}
		else if (param[0] == "/lock") {
			LockBox::RequestLockBox(player);
			player->sendMsg("����Ҫ��������!");
		}
		else if (param[0] == "/g") {
			if (param[1] == "join") {
				if (Guild::isInGuild(player->getNameTag())) {
					player->sendMsg("�㵱ǰ���ڹ�����, ���� /g exit ���˳���ǰ����");
					return true;
				}
				if (param.size() == 3) {
					if (Guild::isGuildSet(param[2])) {
						Guild::RequestJoin(param[2], player->getNameTag());
						string sendoutuuid = NametoUuid[Guild::GetAdmin(param[2])];
						if (sendoutuuid != "") {//�������						
							onlinePlayers[sendoutuuid]->sendMsg("���: " + player->getNameTag() + " ������빫�� " + param[2]);
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
				Guild::AccecptJoin(Guild::PlayerInWhich(player->getNameTag()), player);
			}
			if (param[1] == "create") {
				if (Guild::isInGuild(player->getNameTag())) {
					player->sendMsg("���Ѿ��ڹ�����!");
					player->sendMsg("�������� /g exit ���˳���ǰ����");
				}
				else {
					if (param.size() == 3) {
						if (Economy::GetPlayerMoney(player->getNameTag()) >= 50) {
							Guild::CreateGuild(param[2], player->getNameTag());
							player->sendMsg("�ɹ���������!");
						}
						else {
							player->sendMsg("�������!");
						}
					}
					else {
						player->sendMsg("�÷�: /g create <������> ");

					}
				}
			}
			if (param[1] == "exit") {
				if (Guild::isAdmin(player->getNameTag(), Guild::PlayerInWhich(player->getNameTag()))) {
					Guild::RemoveGuild(Guild::PlayerInWhich(player->getNameTag()), player);
					Guild::ExitGuild(Guild::PlayerInWhich(player->getNameTag()), player);
				}
				else {
					Guild::ExitGuild(Guild::PlayerInWhich(player->getNameTag()), player);
				}
			}
		}
		else if (param[0] == "/chunck" && isAdmin(player)) {
			player->sendMsg("��ǰ�� " + Land::PlayerChunckId(player->getPos()));
		}
		else if (param[0] == "/l") {
			if (param[1] == "buy") {
				if (Economy::GetPlayerMoney(player->getNameTag()) >= 25) {
					if (Land::isLandOwned(Land::PlayerChunckId(player->getPos()))) {
						player->sendMsg("����ѱ�����! ��������� ��a/l info��r �鿴��ϸ��Ϣ");
					}
					else {
						Land::GiveLand(Land::PlayerChunckId(player->getPos()), Guild::getPlayerGuildName(player->getNameTag()));
						Economy::RemovePlayerMoney(player->getNameTag(), 25);
						player->sendMsg("���ѳɹ��������! ��ر��" + Land::PlayerChunckId(player->getPos()));
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
		else {
			return false;
		}
		return true;
	}

	bool static isAdmin(Player* player) {
		return Guild::isInGuild(player->getNameTag(), u8"FutureCraft����Ա");
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
					onlinePlayers[sendoutuuid]->sendMsg("�㵱ǰ���Ϊ: ��l��a" + intToString(Economy::GetPlayerMoney(onlinePlayers[sendoutuuid]->getNameTag())));
				}
			}
			else {
				cout << u8"�÷�: pay <�����> <����>" << endl;
			}
		}
		else {
			return false;
		}
		return true;
	}

};
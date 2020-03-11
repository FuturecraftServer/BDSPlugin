#pragma once
#include "Prebuild.h"
#include <string>
#include <vector>
#include <algorithm>// std::replace_if
#include "BDSAPI.hpp"
#include "Economy.cpp"

#include <time.h>

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
		char buf[64] = { 0 };
		snprintf(buf, sizeof(buf), "%u", v);

		string str = buf;
		return str;
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
		else {
			return false;
		}
		return true;
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
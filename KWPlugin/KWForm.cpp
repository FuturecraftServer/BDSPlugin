#pragma once
#include "Prebuild.h"

#include <string>
#include "Bstream.h"
#include "myPacket.h"
#include "varint.h"
#include <fstream>
#include "BDSAPI.hpp"

using namespace std;
// 1 - custum_form, 2 - modal
static std::map<unsigned, int> fids;

struct FunctionButton {
	bool isconsole = false;//是否为控制台指令
	string command = "NaN";//指令
};

struct FormButton {
	string text = "按钮";
	bool haveimage = false;
	bool isimgurl = false;
	string imgpath;
};
// buttons[formid][buttonid].command
static std::map<unsigned, std::map<int, FunctionButton>> buttons;


static void _sendForm(VA pPlayer, string json, unsigned formid) {
	if (pPlayer == 0)
		return;
	//保存Form
	ofstream SaveFile("log.log");
	SaveFile << json;
	SaveFile.close();
	auto payload = json;
	WBStream ws;
	ws.apply(VarUInt(formid));
	ws.apply(VarUInt(payload.size()));
	ws.write(payload.c_str(), payload.size());
	MyPkt<100, false> guipk{ ws.data };
	SYMCALL(VA, MSSYM_B1QE17sendNetworkPacketB1AE12ServerPlayerB2AAE15UEBAXAEAVPacketB3AAAA1Z,
		pPlayer, &guipk);
}


static void sendRawForm(VA Player, string json, unsigned fid) {
	_sendForm(Player, json, fid);
}

// 销毁已使用的id
static bool destroyForm(unsigned fid)
{
	if (fids[fid]) {
		fids.erase(fid);
		buttons.erase(fid);
		return true;
	}
	return false;
}

static unsigned GenerateFormID() {
	unsigned formid = time(0);
	do {
		formid;
	} while (formid == 0 || fids[formid]);
	fids[formid] = true;
	return formid;
}

struct CustomForm {
private:
	// 发送一个SimpleForm的表单数据包
	void sendForm(VA pPlayer)
	{
		_sendForm(pPlayer, formstring, formid);
	}

public:
	unsigned formid;
	string formstring = "{\"content\":[";
	string ButtonString = "";
	string title = "信息";

	//生成表单,且生成一个ID
	CustomForm() {
		formid = GenerateFormID();
		fids[formid] = 1;
	}

	//添加标签
	void AddLabel(string text) {
		formstring += "{\"type\":\"label\",\"text\":\"" + text + "\"},";
	}

	//设置表单标题
	void SetTitle(string title) {
		this->title = title;
	}

	void releaseForm(VA player) {
		formstring = formstring.substr(0, formstring.length() - 1);
		formstring += "],\"type\": \"custom_form\",\"title\": \"" + title + "\"}";
		sendForm(player);
	}
};

struct ModalForm {
private:
	int buttoncount = 0;
	string formstring = "";
	string ButtonString = "";
	// 发送一个SimpleForm的表单数据包
	void sendForm(VA pPlayer)
	{
		_sendForm(pPlayer, formstring, formid);
	}
public:
	unsigned formid;
	string title = "信息";
	string content = "消息内容";

	//生成表单,且生成一个ID
	ModalForm() {
		formid = GenerateFormID();
		fids[formid] = 2;
	}

	void SetTitle(string title) {
		this->title = title;
	}

	void SetText(string text) {
		this->content = text;
	}

	void AddButton(FormButton fb, FunctionButton func) {
		if (buttoncount != 0) ButtonString += ",";
		ButtonString += "\"button" + std::to_string(buttoncount + 1) + "\":\"" + fb.text + "\"";
		buttons[formid][buttoncount] = func;
		buttoncount++;
	}

	void releaseForm(VA Player) {
		if (buttoncount != 2) ButtonString = u8"\"button1\":\"确认\",\"button2\":\"取消\"";
		formstring = "{\"type\":\"modal\",\"title\":\"" + title + "\",\"content\":\"" + content + "\"," + ButtonString + "}";
		sendForm(Player);
	}
};

struct ButtonsForm {
	int buttonNum = 0;
	string formstring = "";
	string ButtonString = "";
	// 发送一个SimpleForm的表单数据包
	void sendForm(VA pPlayer)
	{
		_sendForm(pPlayer, formstring, formid);
	}
public:
	unsigned formid;
	string title = "信息";
	string content = "消息内容";

	/*
	void SetButtonGroup(std::vector<FormButton> fb, std::vector<FunctionButton> func) {
		formstring += "{\"buttons\": [";
		for (int i = 0; i < fb.size() - 1; i++) {
			formstring += "{\"text\": \"" + fb[i].text + "\"";
			if (fb[i].haveimage) {
				string t = fb[i].isimgurl ? "url" : "path";
				formstring += ",\"image\": { \"type\": \"" + t + "\",\"data\": \"" + fb[i].imgpath + "\"";
			}
			formstring += "},";
			buttons[formid][buttonNum++] = func[i];
		}
		formstring += "{\"text\": \"" + fb[fb.size() - 1].text + "\"";
		if (fb[fb.size() - 1].haveimage) {
			string t = fb[fb.size() - 1].isimgurl ? "url" : "path";
			formstring += ",\"image\": { \"type\": \"" + t + "\",\"data\": \"" + fb[fb.size() - 1].imgpath + "\"";
		}
		formstring += "}";
		buttons[formid][buttonNum++] = func[fb.size() - 1];
		formstring += "]},";
	}
	*/

	//生成表单,且生成一个ID
	ButtonsForm() {
		formid = GenerateFormID();
		fids[formid] = 3;
	}

	void SetTitle(string title) {
		this->title = title;
	}

	void SetText(string text) {
		this->content = text;
	}

	void AddButton(FormButton fb, FunctionButton func) {
		if (buttonNum != 0) ButtonString += ",";
		ButtonString += "{\"text\":\"" + fb.text + "\"";
		if (fb.haveimage) {
			string imgtype = (fb.isimgurl ? "url" : "path");
			ButtonString += ",\"image\":{\"type\":\"" + imgtype + "\",\"data\":\"" + fb.imgpath + "\"";
		}
		ButtonString += "}";
		buttons[formid][buttonNum] = func;
		buttonNum++;
	}

	void releaseForm(VA player) {

		formstring += "{\"type\": \"form\",\"title\": \"" + title + "\",\"content\": \"" + content + "\",\"buttons\":[" + ButtonString + "]}";
		sendForm(player);
	}
};

static void ParseFormCallback(Player* player, unsigned fid, string selected) {
	cout << "Form Select Handling -> Player: " + player->getRealNameTag() + " FormID: " << fid << " Selected: " << selected << " FormType: " << fids[fid] << endl;
	destroyForm(fid);
}


/* Custum Form 不支持按钮
	//添加按钮组
	void SetButtonGroup(std::vector<FormButton> fb, std::vector<FunctionButton> func) {
		formstring += "{\"buttons\": [";
		for (int i = 0; i < fb.size() - 1; i++) {
			formstring += "{\"text\": \"" + fb[i].text + "\"";
			if (fb[i].haveimage) {
				string t = fb[i].isimgurl ? "url" : "path";
				formstring += ",\"image\": { \"type\": \"" + t + "\",\"data\": \"" + fb[i].imgpath + "\"";
			}
			formstring += "},";
			buttons[formid][buttonNum++] = func[i];
		}
		formstring += "{\"text\": \"" + fb[fb.size() - 1].text + "\"";
		if (fb[fb.size() - 1].haveimage) {
			string t = fb[fb.size() - 1].isimgurl ? "url" : "path";
			formstring += ",\"image\": { \"type\": \"" + t + "\",\"data\": \"" + fb[fb.size() - 1].imgpath + "\"";
		}
		formstring += "}";
		buttons[formid][buttonNum++] = func[fb.size() - 1];
		formstring += "]},";
	}
	*/

	/*
	// 创建一个简易表单字符串
	std::string createSimpleFormString(std::string title, std::string content, Json::Value & bttxts) {
		Json::Value jv;
		jv["type"] = "form";
		jv["title"] = title;
		jv["content"] = content;
		jv["buttons"] = bttxts;
		return jv.toStyledString();
	}

	// 创建一个固定模板表单字符串
	std::string createModalFormString(std::string title, std::string content, std::string button1, std::string button2) {
		Json::Value jv;
		jv["type"] = "modal";
		jv["title"] = title;
		jv["content"] = content;
		jv["button1"] = button1;
		jv["button2"] = button2;
		return jv.toStyledString();
	}
	*/

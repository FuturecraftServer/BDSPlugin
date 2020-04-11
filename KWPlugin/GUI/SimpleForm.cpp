#pragma once
#include "../Prebuild.h"

#include <string>
#include "Bstream.h"
#include "myPacket.h"
#include "varint.h"

static std::map<unsigned, bool> fids;

// 获取一个未被使用的基于时间秒数的id
static unsigned getFormId() {
	unsigned id = time(0);
	do {
		--id;
	} while (id == 0 || fids[id]);
	fids[id] = true;
	return id;
}

// 发送一个SimpleForm的表单数据包
unsigned sendForm(VA pPlayer, std::string str)
{
	if (pPlayer == 0)
		return 0;
	auto payload = str;
	WBStream ws;
	unsigned fid = getFormId();
	ws.apply(VarUInt(fid));
	ws.apply(VarUInt(payload.size()));
	ws.write(payload.c_str(), payload.size());
	MyPkt<100, false> guipk{ ws.data };
	SYMCALL(VA, MSSYM_B1QE17sendNetworkPacketB1AE12ServerPlayerB2AAE15UEBAXAEAVPacketB3AAAA1Z,
		pPlayer, &guipk);
	return fid;
}


// 销毁已使用的id
bool destroyForm(unsigned fid)
{
	if (fids[fid]) {
		fids.erase(fid);
		return true;
	}
	return false;
}

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

#pragma once
#include "../Prebuild.h"

#include <string>
#include "Bstream.h"
#include "myPacket.h"
#include "varint.h"

static std::map<unsigned, bool> fids;

// ��ȡһ��δ��ʹ�õĻ���ʱ��������id
static unsigned getFormId() {
	unsigned id = time(0);
	do {
		--id;
	} while (id == 0 || fids[id]);
	fids[id] = true;
	return id;
}

// ����һ��SimpleForm�ı����ݰ�
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


// ������ʹ�õ�id
bool destroyForm(unsigned fid)
{
	if (fids[fid]) {
		fids.erase(fid);
		return true;
	}
	return false;
}

/*
// ����һ�����ױ��ַ���
std::string createSimpleFormString(std::string title, std::string content, Json::Value & bttxts) {
	Json::Value jv;
	jv["type"] = "form";
	jv["title"] = title;
	jv["content"] = content;
	jv["buttons"] = bttxts;
	return jv.toStyledString();
}

// ����һ���̶�ģ����ַ���
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

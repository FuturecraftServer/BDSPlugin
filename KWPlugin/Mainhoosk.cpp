#pragma once
#include "BDSAPI.hpp"

#include<combaseapi.h>

#include <stdio.h>
#include <thread>
#include <mutex>
#include "GUI/SimpleForm.h"

#include "PlayerEvent.cpp"


static bool runcmd(std::string);
static bool reNameByUuid(std::string, std::string);


// ������Ϣ
template<typename T>
static void PR(T arg) {
#ifndef RELEASED
	std::cout << arg << std::endl;
#endif // !RELEASED
}

// ά��IDת��Ϊ�����ַ�
static std::string toDimenStr(int dimensionId) {
	switch (dimensionId) {
	case 0:return u8"������";
	case 1:return u8"����";
	case 2:return u8"ĩ��";
	default:
		break;
	}
	return u8"δ֪ά��";
}

static VA p_level;

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



// �ж�ָ���Ƿ�Ϊ����б���ָ��
static bool checkIsPlayer(void* mp) {
	std::map<std::string, Player*>::iterator iter;
	for (iter = onlinePlayers.begin(); iter != onlinePlayers.end(); iter++) {
		Player* pp = iter->second;
		if (mp == pp)
			return true;
	}
	return false;
}

/*
// ����ָ�����һ����
static UINT sendSimpleForm(char* uuid, char* title, char* content, char* buttons) {
	if (!onlinePlayers[uuid])
		return 0;
	Json::Value bts;
	Json::Value ja = toJson(buttons);
	for (int i = 0; i < ja.size(); i++) {
		Json::Value bt;
		bt["text"] = ja[i];
		bts.append(bt);
	}
	std::string str = createSimpleFormString(title, content, bts);
	return sendForm((VA)onlinePlayers[uuid], str);
}
*/

// ����ָ�����һ��ģ��Ի���
static UINT sendModalForm(char* uuid, char* title, char* content, char* button1, char* button2) {
	if (!onlinePlayers[uuid])
		return 0;
	std::string str = createModalFormString(title, content, button1, button2);
	return sendForm((VA)onlinePlayers[uuid], str);
}

// ����ָ�����һ���Զ���GUI����
static UINT sendCustomForm(char* uuid, char* json) {
	Player* p = onlinePlayers[uuid];
	if (p == NULL)
		return 0;
	return sendForm((VA)p, json);
}

static std::string oldseedstr;

/**************** ���HOOK���� ****************/

// ��׼����������
static const VA STD_COUT_HANDLE = SYM_OBJECT(VA,
	MSSYM_B2UUA3impB2UQA4coutB1AA3stdB2AAA23VB2QDA5basicB1UA7ostreamB1AA2DUB2QDA4charB1UA6traitsB1AA1DB1AA3stdB3AAAA11B1AA1A); //140721370820768


// ��ȡָ�����
THook2(_JS_GETSPSCQUEUE, VA, MSSYM_MD5_3b8fb7204bf8294ee636ba7272eec000,
	VA _this) {
	p_spscqueue = original(_this);
	return p_spscqueue;
}

// ��ȡ��ҳ�ʼ��ʱ��ͼ������Ϣ
THook2(_JS_PLAYERINIT, Player*, MSSYM_MD5_c4b0cddb50ed88e87acce18b5bd3fb8a,
	Player* _this, VA level, __int64 a3, int a4, __int64 a5, __int64 a6, void* uuid, std::string& struuid, __int64* a9, __int64 a10, __int64 a11) {
	p_level = level;
	return original(_this, level, a3, a4, a5, a6, uuid, struuid, a9, a10, a11);
}

// ���������ļ��е�ͼ�����ַ�
THook2(_JS_GETOLDSEEDSTR, UINT, MSSYM_MD5_d2496e689e9641a96868df357e31ad87,
	std::string* pSeedstr, VA a2) {
	oldseedstr = std::string(*pSeedstr);
	return original(pSeedstr, a2);
}

// ��д������Ϸ��Ϣ����������ϢΪ�����ļ�����Ϣ
THook2(_JS_HIDESEEDPACKET, void,
	MSSYM_B1QA5writeB1AE15StartGamePacketB2AAE21UEBAXAEAVBinaryStreamB3AAAA1Z,
	VA _this, VA a2) {
	if (oldseedstr != "") {				// IDA LevelSettings::LevelSettings
		*((UINT*)(_this + 40)) = atoi(oldseedstr.c_str());
	}
	return original(_this, a2);
}

// ��������̨����ָ��

THook2(_JS_ONSERVERCMD, bool,
	MSSYM_MD5_b5c9e566146b3136e6fb37f0c080d91e,
	VA _this, std::string* cmd) {
	return Command::onConsoleSendCommand(*cmd) ? original(_this, cmd) : false;
}

/*
//TODO: Unknown Function Usage
// ��������ָ̨�����
THook2(_JS_ONSERVERCMDOUTPUT, VA,
	MSSYM_MD5_b5f2f0a753fc527db19ac8199ae8f740,
	VA handle, char* str, VA size) {
	if (handle == STD_COUT_HANDLE) {
		Json::Value jv;
		jv["output"] = std::string(str);
		bool ret = runScriptCallBackListener(ActEvent.ONSERVERCMDOUTPUT, ActMode::BEFORE, toJsonString(jv));
		if (ret) {
			VA result = original(handle, str, size);
			jv["result"] = ret;
			runScriptCallBackListener(ActEvent.ONSERVERCMDOUTPUT, ActMode::AFTER, toJsonString(jv));
			return result;
		}
		return handle;
	}
	return original(handle, str, size);
}
*/

//��ҵ��
THook(bool,
	MSSYM_B2QUA5useOnB1AE17FlintAndSteelItemB2AAA4MEBAB1UE14NAEAVItemStackB2AAA9AEAVActorB2AAA9VBlockPosB2AAA4EMMMB1AA1Z,
	__int64 a1, const ItemStack* a2, BlockSource* a3, BlockPos* bp) {
	Player* player = (Player*)a3;
	return PlayerEvent::PlaceFire(player, bp) ? original(a1, a2, a3, bp) : false;
}

// ���ѡ���
THook(void,
	MSSYM_MD5_8b7f7560f9f8353e6e9b16449ca999d2,
	VA _this, VA id, VA handle, ModalFormResponsePacket** fp) {
	ModalFormResponsePacket* fmp = *fp;
	Player* p = SYMCALL(Player*, MSSYM_B2QUE15getServerPlayerB1AE20ServerNetworkHandlerB2AAE20AEAAPEAVServerPlayerB2AAE21AEBVNetworkIdentifierB2AAA1EB1AA1Z,
		handle, id, *(char*)((VA)fmp + 16));
	if (p != NULL) {
		UINT fid = fmp->getFormId();
		if (destroyForm(fid)) {
			PlayerEvent::SelectForm(p, fid, fmp->getSelectStr());
			return;
		}
	}
	original(_this, id, handle, fp);
}


// ��Ҳ�����Ʒ
THook2(_JS_ONUSEITEM, bool,
	MSSYM_B1QA9useItemOnB1AA8GameModeB2AAA4UEAAB1UE14NAEAVItemStackB2AAE12AEBVBlockPosB2AAA9EAEBVVec3B2AAA9PEBVBlockB3AAAA1Z,
	void* _this, ItemStack* item, BlockPos* pBlkpos, unsigned __int8 a4, void* v5, Block* pBlk) {
	auto pPlayer = *reinterpret_cast<Player**>(reinterpret_cast<VA>(_this) + 8);
	return PlayerEvent::UseItem(pPlayer, item, pBlkpos, pBlk) ? original(_this, item, pBlkpos, a4, v5, pBlk) : false;
}


//��Ҳ�����Ʒ��`
THook(void, MSSYM_B1QE14dropFramedItemB1AE19ItemFrameBlockActorB2AAE20QEAAXAEAVBlockSourceB3AAUA1NB1AA1Z,
	void* _this, BlockSource* blk, char* v3) {
	auto real_this = reinterpret_cast<void*>(reinterpret_cast<VA>(_this) - 248);
	auto pBlkpos = reinterpret_cast<BlockActor*>(real_this)->getPosition();
	auto player = *reinterpret_cast<Player**>(reinterpret_cast<VA>(_this) + 8);
	PlayerEvent::BreakItemFrame(player, pBlkpos);
}

//��ҷ��÷���
THook2(_JS_ONPLACEDBLOCK, void,
	MSSYM_B1QE23sendBlockPlacedByPlayerB1AE21BlockEventCoordinatorB2AAE15QEAAXAEAVPlayerB2AAA9AEBVBlockB2AAE12AEBVBlockPosB3AAUA1NB1AA1Z,
	void* _this, Player* pPlayer, const Block* pBlk, BlockPos* pBlkpos) {
	bool ret = PlayerEvent::PlaceBlock(pPlayer, pBlk->getLegacyBlock()->getBlockItemID(), pBlkpos);
	if (ret)
		return original(_this, pPlayer, pBlk, pBlkpos);
	else {
		runcmd("setblock " + pBlkpos->getPosition()->toNormalString() + " air");
		return;
	}
}

/*
// ��ҷ��÷���
THook2(_JS_ONPLACEDBLOCK, __int64,
	MSSYM_B1QE21checkBlockPermissionsB1AE11BlockSourceB2AAA4QEAAB1UE10NAEAVActorB2AAE12AEBVBlockPosB2AAE14EAEBVItemStackB3AAUA1NB1AA1Z,
	void* _this, Player* pPlayer, const Block* pBlk, BlockPos* pBlkpos, bool _bool) {
	bool ret = PlayerEvent::PlaceBlock(pPlayer, pBlk, pBlkpos);
	if (ret)
		return original(_this, pPlayer, pBlk, pBlkpos, _bool);
	else {
		cout << "Try to be canceled" << endl;
		return false;
	}
}
*/

//��ֹ������
THook(bool, MSSYM_B1QE11canPairWithB1AE15ChestBlockActorB2AAA4QEAAB1UE15NPEAVBlockActorB2AAE15AEAVBlockSourceB3AAAA1Z) {
	return false;
}
// ����ƻ�����
THook2(_JS_ONDESTROYBLOCK, bool,
	MSSYM_B2QUE20destroyBlockInternalB1AA8GameModeB2AAA4AEAAB1UE13NAEBVBlockPosB2AAA1EB1AA1Z,
	void* _this, BlockPos* pBlkpos) {
	auto pPlayer = *reinterpret_cast<Player**>(reinterpret_cast<VA>(_this) + 8);
	auto pBlockSource = *(BlockSource**)(*((__int64*)_this + 1) + 840i64);
	auto pBlk = pBlockSource->getBlock(pBlkpos);
	return PlayerEvent::BreakBlock(pPlayer, pBlk, pBlockSource, pBlkpos) ? original(_this, pBlkpos) : false;
}

// ��ҿ���׼��
THook2(_JS_ONCHESTBLOCKUSE, bool,
	MSSYM_B1QA3useB1AE10ChestBlockB2AAA4UEBAB1UE11NAEAVPlayerB2AAE12AEBVBlockPosB3AAAA1Z,
	void* _this, Player* pPlayer, BlockPos* pBlkpos) {
	auto pBlockSource = (BlockSource*)*((__int64*)pPlayer + 105);
	//auto pBlk = pBlockSource->getBlock(pBlkpos);
	return PlayerEvent::ReadyOpenBox(pPlayer, pBlockSource, pBlkpos) ? original(_this, pPlayer, pBlkpos) : false;
}

// ��ҿ�Ͱ׼��
THook2(_JS_ONBARRELBLOCKUSE, bool,
	MSSYM_B1QA3useB1AE11BarrelBlockB2AAA4UEBAB1UE11NAEAVPlayerB2AAE12AEBVBlockPosB3AAAA1Z,
	void* _this, Player* pPlayer, BlockPos* pBlkpos) {
	auto pBlockSource = (BlockSource*)*((__int64*)pPlayer + 105);
	//auto pBlk = pBlockSource->getBlock(pBlkpos);
	return PlayerEvent::ReadyOpenBusket(pPlayer, pBlockSource, pBlkpos) ? original(_this, pPlayer, pBlkpos) : false;
}

// ��ҹر�����
// ��ʱ������Preverse Event
THook2(_JS_ONSTOPOPENCHEST, void,
	MSSYM_B1QA8stopOpenB1AE15ChestBlockActorB2AAE15UEAAXAEAVPlayerB3AAAA1Z,
	void* _this, Player* pPlayer) {
	auto real_this = reinterpret_cast<void*>(reinterpret_cast<VA>(_this) - 248);
	auto pBlkpos = reinterpret_cast<BlockActor*>(real_this)->getPosition();
	PlayerEvent::CloseBox(pPlayer, pBlkpos);
	original(_this, pPlayer);
}

// ��ҹر�ľͰ
THook2(_JS_STOPOPENBARREL, void,
	MSSYM_B1QA8stopOpenB1AE16BarrelBlockActorB2AAE15UEAAXAEAVPlayerB3AAAA1Z,
	void* _this, Player* pPlayer) {
	auto real_this = reinterpret_cast<void*>(reinterpret_cast<VA>(_this) - 248);
	auto pBlkpos = reinterpret_cast<BlockActor*>(real_this)->getPosition();
	PlayerEvent::CloseBusket(pPlayer, pBlkpos);
	original(_this, pPlayer);
}

// ��ҷ���ȡ������
THook2(_JS_ONSETSLOT, void,
	MSSYM_B1QA7setSlotB1AE26LevelContainerManagerModelB2AAE28UEAAXHAEBUContainerItemStackB3AAUA1NB1AA1Z,
	LevelContainerManagerModel* _this, int a2, ContainerItemStack* a3) {
	int slot = a2;
	ContainerItemStack* pItemStack = a3;
	auto pPlayer = _this->getPlayer();
	//VA v3 = *((VA*)_this + 1);				// IDA LevelContainerManagerModel::_getContainer
	//BlockSource* bs = *(BlockSource**)(*(VA*)(v3 + 848) + 72i64);
	BlockPos* pBlkpos = (BlockPos*)((char*)_this + 176);
	if (PlayerEvent::ChestInput(pPlayer, pItemStack, pBlkpos)) {
		original(_this, slot, pItemStack);
	}
	else {
		return;
	}
}

// ����л�ά��
// �ݲ�֧��Preverse Event
THook2(_JS_ONCHANGEDIMENSION, bool,
	MSSYM_B2QUE21playerChangeDimensionB1AA5LevelB2AAA4AEAAB1UE11NPEAVPlayerB2AAE26AEAVChangeDimensionRequestB3AAAA1Z,
	void* _this, Player* pPlayer, void* req) {
	/*
	bool ret = runScriptCallBackListener(ActEvent.ONCHANGEDIMENSION, ActMode::BEFORE, toJsonString(jv));
	if (ret) {
		ret = original(_this, pPlayer, req);
		jv["result"] = ret;
		runScriptCallBackListener(ActEvent.ONCHANGEDIMENSION, ActMode::AFTER, toJsonString(jv));
	}*/
	return original(_this, pPlayer, req);
}

// ������������
THook2(_JS_ONMOBDIE, void,
	MSSYM_B1QA3dieB1AA3MobB2AAE26UEAAXAEBVActorDamageSourceB3AAAA1Z,
	Mob* _this, void* dmsg) {
	/*
	auto mob_name = _this->getNameTag();
	if (mob_name.length() != 0) {
		char v72;
		__int64  v2[2];
		v2[0] = (__int64)_this;
		v2[1] = (__int64)dmsg;
		auto v7 = *((__int64*)(v2[0] + 856));
		auto srActid = (__int64*)(*(__int64(__fastcall**)(__int64, char*))(*(__int64*)v2[1] + 56i64))(
			v2[1],
			&v72);
		auto SrAct = SYMCALL(Actor*,
			MSSYM_B1QE11fetchEntityB1AA5LevelB2AAE13QEBAPEAVActorB2AAE14UActorUniqueIDB3AAUA1NB1AA1Z,
			v7, *srActid, 0);
		std::string sr_name = "";
		if (SrAct) {
			sr_name = SrAct->getNameTag();
		}
		Json::Value jv;
		jv["mobname"] = mob_name;
		if (_this->getEntityTypeId() == 1) {
			// δ֪���ͣ����������
			if (checkIsPlayer(_this)) {
				addPlayerInfo(jv, (Player*)_this);
				std::string playertype;				// IDA Player::getEntityTypeId
				SYMCALL(std::string&, MSSYM_MD5_af48b8a1869a49a3fb9a4c12f48d5a68, &playertype, 319);
				jv["mobtype"] = playertype;			// "entity.player.name"
			}
		}
		else
			jv["mobtype"] = _this->getEntityTypeName();
		jv["srcname"] = sr_name;
		bool ret = runScriptCallBackListener(ActEvent.ONNAMEDMOBDIE, ActMode::BEFORE, toJsonString(jv));
		if (ret) {
			original(_this, dmsg);
			jv["result"] = ret;
			runScriptCallBackListener(ActEvent.ONNAMEDMOBDIE, ActMode::AFTER, toJsonString(jv));
		}
		return;
	}
	*/
	original(_this, dmsg);
}

// �������
THook2(_JS_PLAYERRESPAWN, void, MSSYM_B1QA7respawnB1AA6PlayerB2AAA7UEAAXXZ,
	Player* pPlayer) {
	PlayerEvent::ReSpawn(pPlayer);
	original(pPlayer);
	return;
}

// ������Ϣ
THook2(_JS_ONCHAT, void,
	MSSYM_MD5_ad251f2fd8c27eb22c0c01209e8df83c,
	void* _this, std::string& player_name, std::string& target, std::string& msg, std::string& chat_style) {
	/*
	jv["playername"] = player_name;
	jv["target"] = target;
	jv["msg"] = msg;
	jv["chatstyle"] = chat_style;
	bool ret = runScriptCallBackListener(ActEvent.ONCHAT, ActMode::BEFORE, toJsonString(jv));
	if (ret) {
		original(_this, player_name, target, msg, chat_style);
		jv["result"] = ret;
		runScriptCallBackListener(ActEvent.ONCHAT, ActMode::AFTER, toJsonString(jv));
	}
	*/
	original(_this, player_name, target, msg, chat_style);
}

// �����ı�
THook2(JS_ONINPUTTEXT, void,
	MSSYM_B1QA6handleB1AE20ServerNetworkHandlerB2AAE26UEAAXAEBVNetworkIdentifierB2AAE14AEBVTextPacketB3AAAA1Z,
	VA _this, VA id, TextPacket* tp) {
	Player* player = SYMCALL(Player*, MSSYM_B2QUE15getServerPlayerB1AE20ServerNetworkHandlerB2AAE20AEAAPEAVServerPlayerB2AAE21AEBVNetworkIdentifierB2AAA1EB1AA1Z,
		_this, id, *((char*)tp + 16));
	PlayerEvent::Chat(player, &tp->toString());
}

// ����ָ��
THook2(_JS_ONINPUTCOMMAND, void,
	MSSYM_B1QA6handleB1AE20ServerNetworkHandlerB2AAE26UEAAXAEBVNetworkIdentifierB2AAE24AEBVCommandRequestPacketB3AAAA1Z,
	VA _this, VA id, CommandRequestPacket* crp) {
	Player* p = SYMCALL(Player*, MSSYM_B2QUE15getServerPlayerB1AE20ServerNetworkHandlerB2AAE20AEAAPEAVServerPlayerB2AAE21AEBVNetworkIdentifierB2AAA1EB1AA1Z,
		_this, id, *((char*)crp + 16));
	if (PlayerEvent::Command(p, crp->toString())) original(_this, id, crp);
}

// ��Ҽ�������
// TODO: Unknown Usage
THook2(_JS_ONCREATEPLAYER, Player*,
	MSSYM_B2QUE15createNewPlayerB1AE20ServerNetworkHandlerB2AAE20AEAAAEAVServerPlayerB2AAE21AEBVNetworkIdentifierB2AAE21AEBVConnectionRequestB3AAAA1Z,
	VA a1, VA a2, VA** a3) {
	auto pPlayer = original(a1, a2, a3);
	auto uuid = pPlayer->getUuid()->toString();
	onlinePlayers[uuid] = pPlayer;
	NametoUuid[pPlayer->getRealNameTag()] = uuid;
	return pPlayer;
}

//©����Tick
THook(void,
	MSSYM_B1QA4tickB1AE16HopperBlockActorB2AAE20UEAAXAEAVBlockSourceB3AAAA1Z,
	BlockActor* _this, BlockSource* blocksource) {
	BlockPos* Blkpos = (BlockPos*)((char*)_this + 44);
	if (LockBox::CheckDropper(Blkpos)) {
		original(_this, blocksource);
	}
	else {
		return;
	}
}

// ����뿪��Ϸ
THook2(_JS_ONPLAYERLEFT, void,
	MSSYM_B2QUE12onPlayerLeftB1AE20ServerNetworkHandlerB2AAE21AEAAXPEAVServerPlayerB3AAUA1NB1AA1Z,
	VA _this, Player* pPlayer, char v3) {
	auto uuid = pPlayer->getUuid()->toString();
	NametoUuid.erase(pPlayer->getRealNameTag());
	onlinePlayers[uuid] = NULL;
	onlinePlayers.erase(uuid);
	PlayerEvent::Exit(pPlayer);
	original(_this, pPlayer, v3);
}

// ����ƶ���Ϣ����
THook2(_JS_ONMOVE, __int64,
	MSSYM_B2QQE170MovePlayerPacketB2AAA4QEAAB1AE10AEAVPlayerB2AAE14W4PositionModeB1AA11B1AA2HHB1AA1Z,
	void* _this, Player* pPlayer, char v3, int v4, int v5) {
	
	if (PlayerEvent::Move(pPlayer)) {
		return original(_this, pPlayer, v3, v4, v5);
	}
	else {
		return 0;
	}
	
}

// ��ҹ���ʱ��������
THook2(_JS_ONATTACK, bool,
	MSSYM_B1QA6attackB1AA6PlayerB2AAA4UEAAB1UE10NAEAVActorB3AAAA1Z,
	Player* class_this, Actor* pactor) {
	return PlayerEvent::Attack(class_this, pactor) ? original(class_this, pactor) : false;
}



#pragma endregion

void init() {
	std::cout << u8"Init KWPlugin V1.0.4 (branch Master) FutureCraft Original" << std::endl << "Path: " << getLocalPath();
	std::cout << u8"Copyright Kengwang All rights reserved" << std::endl;
}




void exit() {
}


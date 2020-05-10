#pragma once
#include "Prebuild.h" //Prebuild Header
#include "BDSAPI.hpp"
#include<combaseapi.h>

#include <stdio.h>
#include <thread>
#include <mutex>

#include "PlayerEvent.cpp"
#include "Event.cpp"


static bool runcmd(std::string);
static bool reNameByUuid(std::string, std::string);
static std::map<std::string, bool> canUnpairChest;


// 维度ID转换为中文字符
static std::string toDimenStr(int dimensionId) {
	switch (dimensionId) {
	case 0:return "主世界";
	case 1:return "地狱";
	case 2:return "末地";
	default:
		break;
	}
	return "未知维度";
}

static VA p_level;

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



// 判断指针是否为玩家列表中指针
static bool checkIsPlayer(void* mp) {
	std::map<std::string, Player*>::iterator iter;
	for (iter = onlinePlayers.begin(); iter != onlinePlayers.end(); iter++) {
		Player* pp = iter->second;
		if (mp == pp)
			return true;
	}
	return false;
}

static std::string oldseedstr;

/**************** 插件HOOK区域 ****************/

// 标准输出句柄常量
static const VA STD_COUT_HANDLE = SYM_OBJECT(VA,
	MSSYM_B2UUA3impB2UQA4coutB1AA3stdB2AAA23VB2QDA5basicB1UA7ostreamB1AA2DUB2QDA4charB1UA6traitsB1AA1DB1AA3stdB3AAAA11B1AA1A); //140721370820768


// 获取指令队列
THook2(_JS_GETSPSCQUEUE, VA, MSSYM_MD5_3b8fb7204bf8294ee636ba7272eec000,
	VA _this) {
	p_spscqueue = original(_this);
	return p_spscqueue;
}

// 获取玩家初始化时地图基本信息
THook2(_JS_PLAYERINIT, Player*, MSSYM_MD5_c4b0cddb50ed88e87acce18b5bd3fb8a,
	Player* _this, VA level, __int64 a3, int a4, __int64 a5, __int64 a6, void* uuid, std::string& struuid, __int64* a9, __int64 a10, __int64 a11) {
	p_level = level;
	return original(_this, level, a3, a4, a5, a6, uuid, struuid, a9, a10, a11);
}


// 保存设置文件中地图种子字符
THook2(_JS_GETOLDSEEDSTR, UINT, MSSYM_MD5_d2496e689e9641a96868df357e31ad87,
	std::string* pSeedstr, VA a2) {
	oldseedstr = std::string(*pSeedstr);
	return original(pSeedstr, a2);
}

// 改写发信游戏信息包中种子信息为设置文件中信息
THook2(_JS_HIDESEEDPACKET, void,
	MSSYM_B1QA5writeB1AE15StartGamePacketB2AAE21UEBAXAEAVBinaryStreamB3AAAA1Z,
	VA _this, VA a2) {
	if (oldseedstr != "") {				// IDA LevelSettings::LevelSettings
		*((UINT*)(_this + 40)) = atoi(oldseedstr.c_str());
	}
	return original(_this, a2);
}

// 服务器后台输入指令

THook(bool,
	MSSYM_MD5_b5c9e566146b3136e6fb37f0c080d91e,
	VA _this, std::string* cmd) {
	return Command::onConsoleSendCommand(*cmd) ? original(_this, cmd) : false;
}

/*
//TODO: Unknown Function Usage
// 服务器后台指令输出
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

//玩家点火
THook(bool,
	MSSYM_B2QUA5useOnB1AE17FlintAndSteelItemB2AAA4MEBAB1UE14NAEAVItemStackB2AAA9AEAVActorB2AAA9VBlockPosB2AAA4EMMMB1AA1Z,
	__int64 a1, const ItemStack* a2, BlockSource* a3, BlockPos* bp) {
	Player* player = (Player*)a3;
	return PlayerEvent::PlaceFire(player, bp) ? original(a1, a2, a3, bp) : false;
}

// 玩家选择表单
THook(void,
	MSSYM_MD5_8b7f7560f9f8353e6e9b16449ca999d2,
	VA _this, VA id, VA handle, ModalFormResponsePacket** fp) {
	ModalFormResponsePacket* fmp = *fp;
	Player* p = SYMCALL(Player*, MSSYM_B2QUE15getServerPlayerB1AE20ServerNetworkHandlerB2AAE20AEAAPEAVServerPlayerB2AAE21AEBVNetworkIdentifierB2AAA1EB1AA1Z,
		handle, id, *(char*)((VA)fmp + 16));
	if (p != NULL) {
		UINT fid = fmp->getFormId();
		// TODO
		//if (destroyForm(fid)) {
		PlayerEvent::ParseFormCallback(p, fid, fmp->getSelectStr());
		return;
		//}
	}
	original(_this, id, handle, fp);
}

//服务器MOTD加载
THook(void,
	MSSYM_MD5_21204897709106ba1d290df17fead479,
	VA _this, string& servername, bool shouldannounce) {
	ServerNetworkHandler = _this;
	//这个纯粹是抓取servernetworkhandler的!
	original(_this, servername, shouldannounce);
}

// 玩家操作物品
THook(bool,
	MSSYM_B1QA9useItemOnB1AA8GameModeB2AAA4UEAAB1UE14NAEAVItemStackB2AAE12AEBVBlockPosB2AAA9EAEBVVec3B2AAA9PEBVBlockB3AAAA1Z,
	void* _this, ItemStack* item, BlockPos* pBlkpos, unsigned __int8 a4, void* v5, Block* pBlk) {
	auto pPlayer = *reinterpret_cast<Player**>(reinterpret_cast<VA>(_this) + 8);
	return PlayerEvent::UseItem(pPlayer, item, pBlkpos, pBlk) ? original(_this, item, pBlkpos, a4, v5, pBlk) : false;
}


//玩家使物品展示框掉落
THook(void, MSSYM_B1QE17playerWillDestroyB1AE14ItemFrameBlockB2AAA4UEBAB1UE11NAEAVPlayerB2AAE12AEBVBlockPosB2AAA9AEBVBlockB3AAAA1Z,
	Block* _this, Player* a2, BlockPos* a3, Block* a4) {
	PlayerEvent::BreakItemFrame(a2, a3);
}

//玩家放置方块
THook(bool,
	MSSYM_B1QA8mayPlaceB1AE11BlockSourceB2AAA4QEAAB1UE10NAEBVBlockB2AAE12AEBVBlockPosB2AAE10EPEAVActorB3AAUA1NB1AA1Z,
	BlockSource* _this, Block* a2, BlockPos* a3, unsigned __int8 a4, Player* a5, bool a6) {
	bool ret = false;
	if (checkIsPlayer(a5)) {
		ret = PlayerEvent::PlaceBlock(a5, a2, a3);
	}
	else {
		ret = true;
	}
	if (ret)
		return original(_this, a2, a3, a4, a5, a6);
	else {
		return false;;
	}
}

/*
// 玩家放置方块
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


//大箱子合并
THook(void,
	MSSYM_B1QA8pairWithB1AE15ChestBlockActorB2AAE10QEAAXPEAV1B2AUA1NB1AA1Z,
	BlockActor* _this, BlockActor* a2, char a3) {
	//cout << "Checking " << _this->getPosition()->getPosition()->toNormalString() << " can compair with " << a2->getPosition()->getPosition()->toNormalString() << endl;
	original(_this, a2, a3);
	string pos = _this->getPosition()->getPosition()->toNormalString();
	if (LockBox::isLockBox(pos)) {
		LockBox::SetPermission(LockBox::GetOwner(pos), a2->getPosition()->getPosition()->toNormalString());
	}

}

//生物自动生成
THook(bool,
	MSSYM_B1QE15checkSpawnRulesB1AA3MobB2AAA4UEAAB1UA1NB1UA1NB1AA1Z,
	Mob* _this, bool a2) {
	//cout << "Checking can spawn " << _this->getEntityTypeId() << " (" << _this->getEntityTypeName() << ") at " << _this->getPos()->toNormalString();
	if (Event::canSpawn(_this)) {
		//cout << " >>> can spawn" << endl;
		return original(_this, a2);
	}
	else {
		//cout << " >>> no spawn" << endl;
		return false;
	}
}

/* 废弃无用
//修改玩家权限
THook(void,
	MSSYM_B1QE20setPlayerPermissionsB1AA9AbilitiesB2AAE28QEAAXW4PlayerPermissionLevelB3AAAA1Z,
	VA _this, unsigned int level) {
	cout << "Setting Level to " << level << endl;
	original(_this, level);
}
*/

/*无用
//修改玩家成员属性
THook(void,
	MSSYM_B1QA7executeB1AE14AbilityCommandB2AAE22UEBAXAEBVCommandOriginB2AAE17AEAVCommandOutputB3AAAA1Z,
	VA* _this, PlayerCommandOrigin* a2, VA* a3) {
	int* alilityIndex = (int*)((char*)_this + 200);
	int value=*((char*)_this + 232);
	cout << "Player: " << a2->getName() << " trys to set ability " << alilityIndex << " to " << value << endl;
	original(_this, a2, a3);
}
*/

//玩家更改游戏模式
THook(void,
	MSSYM_B1QE17setPlayerGameTypeB1AA6PlayerB2AAE15UEAAXW4GameTypeB3AAAA1Z,
	Player* _this, int gametype) {
	//0 - 生存模式    1 - 创造模式    2 - 冒险模式  3 -    5 - 默认
	//cout << "Player " << _this->getRealNameTag() << " is changing gametype to " << gametype;
	if (Command::isAdmin(_this)) {
		original(_this, gametype);
	}
	else {
		_this->sendMsg("无法修改您的游戏模式,因为你不在管理员公会 (已将你的游戏模式设置成生存)");
		original(_this, 0);
	}
}

/*

//玩家选择物品
THook(void,
	MSSYM_B1QE10selectItemB1AE12ServerPlayerB2AAE18QEAAXAEBVItemStackB2AAA4AEBHB1AA1Z,
	Player* _this, ItemStack* item, int* maxNumSlots) {
	if (PlayerEvent::MouseClickItem(_this, item)) {
		original(_this,  item, maxNumSlots);
	}
	else {
		cout << "Find Somebody Cheating" << endl;
		item->set(0);
	}
}
*/

THook(int,
	MSSYM_B1QE15getEnchantLevelB1AE12EnchantUtilsB2AAA9SAHW4TypeB1AA7EnchantB2AAE17AEBVItemStackBaseB3AAAA1Z,
	int a1, ItemStack* a2) {
	int ori = original(a1, a2);
	//cout << "Checking Enchaned item " << a2->getName() << " type: " << a1 << " level: " << ori << endl;
	if (!Event::checkItemEnchantLevel(a2, a1, ori)) {
		a2->set(0);
		return 1;
	}
	return ori;
}


/* 有BUG,废弃
// 玩家破坏方块
THook(bool,
	MSSYM_B1QE12destroyBlockB1AA8GameModeB2AAA4UEAAB1UE13NAEBVBlockPosB2AAA1EB1AA1Z,
	void* _this, BlockPos* pBlkpos, bool a3) {
	Player* pPlayer = reinterpret_cast<Player*>(reinterpret_cast<VA>(_this) + 1);
	BlockSource* pBlockSource = reinterpret_cast<BlockSource*>(reinterpret_cast<VA>(_this) + 801);
	auto pBlk = pBlockSource->getBlock(pBlkpos);
	if (PlayerEvent::BreakBlock(pPlayer, pBlk, pBlkpos)) {
		return original(_this, pBlkpos, a3);
	}
	else {
		return false;
	}
}
*/

//玩家破坏方块
THook(bool,
	MSSYM_B1QE28checkBlockDestroyPermissionsB1AE11BlockSourceB2AAA4QEAAB1UE10NAEAVActorB2AAE12AEBVBlockPosB2AAE13AEBVItemStackB3AAUA1NB1AA1Z,
	BlockSource* _this, Player* a2, BlockPos* a3, ItemStack* a4, bool a5) {
	if (checkIsPlayer(a2)) {
		if (PlayerEvent::BreakBlock(a2, _this->getBlock(a3), a3)) {
			return original(_this, a2, a3, a4, a5);
		}
		else {
			return false;
		}
	}
	else {
		return original(_this, a2, a3, a4, a5);

	}

}

//箱子被移除
THook(void,
	MSSYM_B1QA8onRemoveB1AE10ChestBlockB2AAE20UEBAXAEAVBlockSourceB2AAE12AEBVBlockPosB3AAAA1Z,
	Block* _this, BlockSource* a2, BlockPos* a3) {
	if (LockBox::isLockBox(a3->getPosition()->toNormalString())) {
		return;
	}
	else {
		original(_this, a2, a3);
	}
}

// 玩家开箱准备
THook(bool,
	MSSYM_B1QA3useB1AE10ChestBlockB2AAA4UEBAB1UE11NAEAVPlayerB2AAE12AEBVBlockPosB3AAAA1Z,
	void* _this, Player* pPlayer, BlockPos* pBlkpos) {
	auto pBlockSource = (BlockSource*)*((__int64*)pPlayer + 100);
	((ChestBlockActor*)(pBlockSource->getBlockEntity(pBlkpos)))->tick(pBlockSource);
	//auto pBlk = pBlockSource->getBlock(pBlkpos);
	return PlayerEvent::ReadyOpenBox(pPlayer, pBlockSource, pBlkpos) ? original(_this, pPlayer, pBlkpos) : false;
}

/*
//箱子是否可以打开
THook(bool,
	MSSYM_B2QUE11canOpenThisB1AE15ChestBlockActorB2AAA4MEBAB1UE16NAEAVBlockSourceB3AAAA1Z,
	ChestBlockActor* _this, struct BlockSource* a2) {
	//_this->tick(a2);
	return original(_this, a2);
}*/

//箱子解除合并
THook(void,
	MSSYM_B1QA6unpairB1AE15ChestBlockActorB2AAE20QEAAXAEAVBlockSourceB3AAAA1Z,
	ChestBlockActor* _this, BlockSource* a2)
{
	if (LockBox::isLockBox(_this->getPosition()->getPosition()->toNormalString())) {
		return;
	}
	original(_this, a2);
	/*
		ChestBlockActor* v4=(ChestBlockActor*) a2->getBlockEntity(*((char*)_this + 134), *((char*)_this + 12), *((char*)_this + 135));
		_this->pairWith(v4, ~*((char*)v4 + 532) & 1);
		*/

}

// 玩家开桶准备
THook2(_JS_ONBARRELBLOCKUSE, bool,
	MSSYM_B1QA3useB1AE11BarrelBlockB2AAA4UEBAB1UE11NAEAVPlayerB2AAE12AEBVBlockPosB3AAAA1Z,
	void* _this, Player* pPlayer, BlockPos* pBlkpos) {
	auto pBlockSource = (BlockSource*)*((__int64*)pPlayer + 105);
	//auto pBlk = pBlockSource->getBlock(pBlkpos);
	return PlayerEvent::ReadyOpenBusket(pPlayer, pBlockSource, pBlkpos) ? original(_this, pPlayer, pBlkpos) : false;
}

// 玩家关闭箱子
// 暂时不允许Preverse Event
THook2(_JS_ONSTOPOPENCHEST, void,
	MSSYM_B1QA8stopOpenB1AE15ChestBlockActorB2AAE15UEAAXAEAVPlayerB3AAAA1Z,
	void* _this, Player* pPlayer) {
	auto real_this = reinterpret_cast<void*>(reinterpret_cast<VA>(_this) - 248);
	auto pBlkpos = reinterpret_cast<BlockActor*>(real_this)->getPosition();
	PlayerEvent::CloseBox(pPlayer, pBlkpos);
	original(_this, pPlayer);
}

// 玩家关闭木桶
THook2(_JS_STOPOPENBARREL, void,
	MSSYM_B1QA8stopOpenB1AE16BarrelBlockActorB2AAE15UEAAXAEAVPlayerB3AAAA1Z,
	void* _this, Player* pPlayer) {
	auto real_this = reinterpret_cast<void*>(reinterpret_cast<VA>(_this) - 248);
	auto pBlkpos = reinterpret_cast<BlockActor*>(real_this)->getPosition();
	PlayerEvent::CloseBusket(pPlayer, pBlkpos);
	original(_this, pPlayer);
}

// 玩家放入取出数量
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

// 玩家切换维度
// 暂不支持Preverse Event
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

//加载指令
THook(void,
	MSSYM_B1QE13initCoreEnumsB1AE17MinecraftCommandsB2AAA5QEAAXB1UE20NAEBVBaseGameVersionB3AAAA1Z,
	MinecraftCommands* a0, VA a1, VA a2) {
	commands = a0;
	original(a0, a1, a2);
}

// 命名生物死亡
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
			// 未知类型，可能是玩家
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

// 玩家重生
THook2(_JS_PLAYERRESPAWN, void, MSSYM_B1QA7respawnB1AA6PlayerB2AAA7UEAAXXZ,
	Player* pPlayer) {
	PlayerEvent::ReSpawn(pPlayer);
	original(pPlayer);
	return;
}

// 聊天消息
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

// 输入文本
THook2(JS_ONINPUTTEXT, void,
	MSSYM_B1QA6handleB1AE20ServerNetworkHandlerB2AAE26UEAAXAEBVNetworkIdentifierB2AAE14AEBVTextPacketB3AAAA1Z,
	VA _this, VA id, TextPacket* tp) {
	Player* player = SYMCALL(Player*, MSSYM_B2QUE15getServerPlayerB1AE20ServerNetworkHandlerB2AAE20AEAAPEAVServerPlayerB2AAE21AEBVNetworkIdentifierB2AAA1EB1AA1Z,
		_this, id, *((char*)tp + 16));
	PlayerEvent::Chat(player, &tp->toString());
}


// 输入指令
THook2(_JS_ONINPUTCOMMAND, void,
	MSSYM_B1QA6handleB1AE20ServerNetworkHandlerB2AAE26UEAAXAEBVNetworkIdentifierB2AAE24AEBVCommandRequestPacketB3AAAA1Z,
	VA _this, VA id, CommandRequestPacket* crp) {
	Player* p = SYMCALL(Player*, MSSYM_B2QUE15getServerPlayerB1AE20ServerNetworkHandlerB2AAE20AEAAPEAVServerPlayerB2AAE21AEBVNetworkIdentifierB2AAA1EB1AA1Z,
		_this, id, *((char*)crp + 16));
	if (PlayerEvent::Command(p, crp->toString())) original(_this, id, crp);
}


// 玩家加载名字
THook2(_JS_ONCREATEPLAYER, Player*,
	MSSYM_B2QUE15createNewPlayerB1AE20ServerNetworkHandlerB2AAE20AEAAAEAVServerPlayerB2AAE21AEBVNetworkIdentifierB2AAE21AEBVConnectionRequestB3AAAA1Z,
	VA a1, VA a2, VA** a3) {
	auto pPlayer = original(a1, a2, a3);
	auto uuid = pPlayer->getUuid()->toString();
	onlinePlayers[uuid] = pPlayer;
	NametoUuid[pPlayer->getRealNameTag()] = uuid;
	return pPlayer;
}


//漏斗的Tick
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

//活塞推方块
THook(bool,
	MSSYM_B2QUE19attachedBlockWalkerB1AE16PistonBlockActorB2AAA4AEAAB1UE16NAEAVBlockSourceB2AAE12AEBVBlockPosB2AAA2EEB1AA1Z,
	BlockActor* _this, BlockSource* a2, BlockPos* a3, unsigned __int8 a4, unsigned __int8 a5) {
	Block* pushingBlock = a2->getBlock(a3);
	//BlockPos* pos = a3;
	short bid = pushingBlock->getLegacyBlock()->getBlockItemID();
	if (bid == -198 || bid == -196 || bid == 61 || bid == 23 || bid == 58 || bid == 125) {
		return false;
	}
	if (bid == 54) {//推箱子
		if (LockBox::isLockBox(a3->getPosition()->toNormalString())) {
			return false;
		}
	}
	return original(_this, a2, a3, a4, a5);
}
//

// 玩家离开游戏
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

// 玩家移动信息构筑
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

// 玩家攻击时触发调用
THook2(_JS_ONATTACK, bool,
	MSSYM_B1QA6attackB1AA6PlayerB2AAA4UEAAB1UE10NAEAVActorB3AAAA1Z,
	Player* class_this, Actor* pactor) {
	return PlayerEvent::Attack(class_this, pactor) ? original(class_this, pactor) : false;
}



#pragma endregion

void init() {
	std::cout << "Init KWPlugin V1.0.7 Release (branch Master)" << std::endl << "For Minecraft Bedrock Edition 1.14.60.5" << std::endl << "Build: " << __DATE__ << " " << __TIME__ << endl << "Path: " << getLocalPath() << endl;
	std::cout << "Copyright @Kengwang  All rights reserved" << std::endl;
	cout << "Loading Plugin Settings" << endl;
	AdminGuild = CConfig::GetValueString("Settings", "Settings", "AdminGuild", "FutureCraft管理员");
	cout << "Plugin Settings Done! Have Fun~" << endl;
}




void exit() {
}


#pragma once
#include "Prebuild.h"

#include "varint.h"
#include "Bstream.h"
#include "myPacket.h"
#include <locale.h>
#include <codecvt> 
#include <wchar.h>

static VA p_spscqueue;
static VA ServerNetworkHandler;
static string AdminGuild;

//UTF-8 转 GBK
static std::string UTF8ToGBK(const char* strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}

// 执行后端指令
static bool runcmd(std::string cmd) {
	if (p_spscqueue != 0) {
		return SYMCALL(bool, MSSYM_MD5_b5c9e566146b3136e6fb37f0c080d91e, p_spscqueue, cmd);
	}
	return false;
}

static void changeMOTD(std::string motd) {
	SYMCALL(void,
		MSSYM_MD5_21204897709106ba1d290df17fead479,
		ServerNetworkHandler, &motd, true);
}



/* Color Code */
const string TEXTFORMATE_BLACK = "§0";
const string TEXTFORMATE_DARKBLUE = "§1";
const string TEXTFORMATE_DARKGREEN = "§2";
const string TEXTFORMATE_DARKAQUA = "§3";
const string TEXTFORMATE_DARKRED = "§4";
const string TEXTFORMATE_DARKPURPLE = "§5";
const string TEXTFORMATE_GLOD = "§6";
const string TEXTFORMATE_GREY = "§7";
const string TEXTFORMATE_DARKGREY = "§8";
const string TEXTFORMATE_BLUE = "§9";
const string TEXTFORMATE_GREEN = "§a";
const string TEXTFORMATE_AUQA = "§b";
const string TEXTFORMATE_RED = "§c";
const string TEXTFORMATE_LIGHTPURPLE = "§d";
const string TEXTFORMATE_YELLOW = "§e";
const string TEXTFORMATE_WHITE = "§f";

/* Format Code */
const string TEXTFORMATE_RANDOM = "§k";
const string TEXTFORMATE_BOLD = "§l";
const string TEXTFORMATE_DELETELINE = "§m";
const string TEXTFORMATE_UNDERLINE = "§n";
const string TEXTFORMATE_ITALY = "§o";
const string TEXTFORMATE_RESET = "§r";
const string TEXTFORMATE_NEWLINE = "\\n";

std::string static stringToUTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}




std::string static replace_all_distinct(std::string str, std::string old_value, std::string new_value)
{
	for (std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length()) {
		if ((pos = str.find(old_value, pos)) != std::string::npos)
			str.replace(pos, old_value.length(), new_value);
		else   break;
	}
	return   str;
}

struct CommandOrigin {

};

struct Minecraft {
};

struct MinecraftCommands;
static MinecraftCommands* commands;
struct MinecraftCommands {
	bool static _runcmd(void* origin, const string& cmd, int unk1, bool unk2) {
		/*SymCall("?executeCommand@MinecraftCommands@@QEBA?AUMCRESULT@@V?$shared_ptr@VCommandContext@@@std@@_N@Z", void, MinecraftCommands*, MCRESULT*, shared_ptr<CommandContext>, bool)(LocateS<MinecraftCommands>::_srv,&rv,std::make_shared<CommandContext>(std::forward<TP>(cmd),(CommandOrigin*)origin),false);*/
		//SymCall("?requestCommandExecution@MinecraftCommands@@QEBA?AUMCRESULT@@V?$unique_ptr@VCommandOrigin@@U?$default_delete@VCommandOrigin@@@std@@@std@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@4@H_N@Z", void, MinecraftCommands*, MCRESULT*, void**, string const&, int, bool)(LocateS<MinecraftCommands>::_srv, &rv, &origin, cmd, unk1, unk2);
		bool rv;
		SYMCALL(void,
			MSSYM_MD5_2f44106d21f04bf0ef021570ea279df0,
			commands, &rv, &origin, cmd, unk1, unk2);
		return rv;
	}
};

// 方块坐标结构体
struct BPos3 {
	INT32 x;
	INT32 y;
	INT32 z;
	std::string toJsonString() {
		char str[256];
		sprintf_s(str, "{\"x\":%d,\"y\":%d,\"z\":%d}", x, y, z);
		return std::string(str);
	}

	std::string toNormalString(int xp = 0, int yp = 0, int zp = 0) {
		char str[256];
		sprintf_s(str, "%d %d %d", x + xp, y + yp, z + zp);
		return std::string(str);
	}
};

struct BlockLegacy {
	// 获取方块名
	auto getFullName() const {				// IDA BlockLegacy::~BlockLegacy
		return (std::string&) * (__int64*)((__int64)this + 104);
	}
	// 获取方块ID号
	auto getBlockItemID() const {
		return SYMCALL(short,
			MSSYM_B1QE14getBlockItemIdB1AE11BlockLegacyB2AAA7QEBAFXZ,
			this);
	}
};

struct BlockPos {
	// 获取坐标数组头
	BPos3* getPosition() const {
		return reinterpret_cast<BPos3*>(reinterpret_cast<VA>(this));
	}
};

struct Block {
	// 获取源
	const BlockLegacy* getLegacyBlock() const {
		return SYMCALL(const BlockLegacy*,
			MSSYM_B1QE14getLegacyBlockB1AA5BlockB2AAE19QEBAAEBVBlockLegacyB2AAA2XZ,
			this);
	}
};

struct BlockActor {
	// 取方块
	Block* getBlock() {
		return *reinterpret_cast<Block**>(reinterpret_cast<VA>(this) + 16);
	}
	// 取方块位置
	BlockPos* getPosition() {				// IDA BlockActor::BlockActor
		return reinterpret_cast<BlockPos*>(reinterpret_cast<VA>(this) + 44);
	}
};


struct BlockSource {
	// 取方块
	Block* getBlock(const BlockPos* blkpos) {
		return SYMCALL(Block*,
			MSSYM_B1QA8getBlockB1AE11BlockSourceB2AAE13QEBAAEBVBlockB2AAE12AEBVBlockPosB3AAAA1Z,
			this, blkpos);
	}

	BlockActor* getBlockEntity(int a2, __int16 a3, int a4) {
		return SYMCALL(BlockActor*,
			MSSYM_B1QE14getBlockEntityB1AE11BlockSourceB2AAE18QEAAPEAVBlockActorB2AAA3HHHB1AA1Z,
			this, a2, a3, a4);
	}
	BlockActor* getBlockEntity(BlockPos* a2) {
		return SYMCALL(BlockActor*,
			MSSYM_B1QE14getBlockEntityB1AE11BlockSourceB2AAE18QEAAPEAVBlockActorB2AAE12AEBVBlockPosB3AAAA1Z,
			this, a2);
	}
};

struct ChestBlockActor :BlockActor {
	void tick(BlockSource* a2) {
		SYMCALL(void,
			MSSYM_B1QA4tickB1AE15ChestBlockActorB2AAE20UEAAXAEAVBlockSourceB3AAAA1Z,
			this, a2);
	}

	void pairWith(ChestBlockActor* a2, char a3) {
		SYMCALL(void,
			MSSYM_B1QA4tickB1AE15ChestBlockActorB2AAE20UEAAXAEAVBlockSourceB3AAAA1Z,
			this, a2, a3);
	}
};

struct MCUUID {
	// 取uuid字符串
	std::string toString() {
		std::string s;
		SYMCALL(std::string&, MSSYM_MD5_40e8abf6eb08f7ee446159cdd0a7f283, this, &s);
		return s;
	}
};

// 玩家坐标结构体
struct Vec3 {
	float x;
	float y;
	float z;

	std::string toJsonString() {
		char str[256];
		sprintf_s(str, "{\"x\":%.2f,\"y\":%.2f,\"z\":%.2f}", x, y, z);
		return std::string(str);
	}

	std::string toNormalString(int xp = 0, int yp = 0, int zp = 0) {
		return std::to_string((int)x) + " " + std::to_string((int)y) + " " + std::to_string((int)z);
	}

};


struct ActorUniqueID {
	unsigned long id;
	ActorUniqueID() { id = -1; }
};

struct Actor {
	// 取方块源
	BlockSource* getRegion() {
		return *reinterpret_cast<BlockSource**>(reinterpret_cast<VA>(this) + 414 * sizeof(void*));
	}

	// 获取生物名称信息
	std::string getNameTag() {
		return SYMCALL(std::string&,
			MSSYM_MD5_7044ab83168b0fd345329e6566fd47fd,
			this);
		//return ;
	}

	// 获取生物当前所处维度ID
	int getDimensionId() {
		int dimensionId;
		SYMCALL(int&,
			MSSYM_B1QE14getDimensionIdB1AA5ActorB2AAA4UEBAB1QA2AVB2QDE11AutomaticIDB1AE10VDimensionB2AAA1HB2AAA2XZ,
			this, &dimensionId);
		return dimensionId;
	}

	// 是否悬空
	const BYTE isStand() {				// IDA MovePlayerPacket::MovePlayerPacket
		return *reinterpret_cast<BYTE*>(reinterpret_cast<VA>(this) + 376);
	}

	// 获取生物当前所在坐标
	Vec3* getPos() {
		return SYMCALL(Vec3*,
			MSSYM_B1QA6getPosB1AA5ActorB2AAE12UEBAAEBVVec3B2AAA2XZ, this);
	}

	// 获取生物类型
	std::string getTypeName() {
		std::string actor_typename;
		SYMCALL(std::string&,
			MSSYM_MD5_01064f7d893d9f9ef50acf1f931d1d79,
			&actor_typename, this);
		return actor_typename;
	}

	// 获取实体类型
	int getEntityTypeId() {
		return SYMCALL(int,
			MSSYM_B1QE15getEntityTypeIdB1AA5ActorB2AAA4UEBAB1QE12AW4ActorTypeB2AAA2XZ,
			this);
		//		if (t == 1)		// 未知类型，可能是玩家
		//			return 319;
	}

	// 获取实体名称
	std::string getEntityTypeName() {
		std::string en_name;
		SYMCALL(std::string&,
			MSSYM_MD5_af48b8a1869a49a3fb9a4c12f48d5a68,
			&en_name, getEntityTypeId());
		return en_name;
	}

	// 获取UniqueId
	ActorUniqueID getUniqueID() {
		return SYMCALL(ActorUniqueID,
			MSSYM_B1QE11getUniqueIDB1AA5ActorB2AAE21QEBAAEBUActorUniqueIDB2AAA2XZ,
			this);
	}
};
struct Mob : Actor {

};

struct Item {

	// 取物品名称
	std::string getName() {
		std::string str;
		SYMCALL(__int64,
			MSSYM_MD5_6d581a35d7ad70fd364b60c3ebe93394,
			this, &str);
		return str;
	}


};

struct ItemStack {

	string getDescriptionId() {
		std::string str;
		SYMCALL(VA,
			MSSYM_MD5_bd9055ba5b9a3d9d0513b31a8fccae4b,
			this, &str);
		return str;
	}


	string getRawNameId() {
		std::string str;
		SYMCALL(VA,
			MSSYM_MD5_2f9d68ca736b0da0c26f063f568898bc,
			this, &str);
		return str;
	}

	// 取物品ID
	short getId() {
		return SYMCALL(short,
			MSSYM_B1QA5getIdB1AE13ItemStackBaseB2AAA7QEBAFXZ,
			this);
	}

	Item* getItem() {
		return SYMCALL(Item*,
			MSSYM_B1QA7getItemB1AE13ItemStackBaseB2AAE12QEBAPEBVItemB2AAA2XZ,
			this);
	}


	// 取物品特殊值
	short getAuxValue() {
		return SYMCALL(short,
			MSSYM_B1QE11getAuxValueB1AE13ItemStackBaseB2AAA7QEBAFXZ,
			this);
	}
	// 取物品名称
	std::string getName() {
		std::string str;
		SYMCALL(__int64,
			MSSYM_MD5_6d581a35d7ad70fd364b60c3ebe93394,
			this, &str);
		return str;
	}
	// 取容器内数量
	int getStackSize() {
		return SYMCALL(int,
			MSSYM_B1QA8getCountB1AE18ContainerItemStackB2AAA7QEBAHXZ,
			this);
	}
	// 判断是否空容器
	bool isNull() {
		return SYMCALL(bool,
			MSSYM_B1QA6isNullB1AE13ItemStackBaseB2AAA4QEBAB1UA3NXZ,
			this);
	}
};

struct Level {

};



static void dummy() {
}
static void* FAKE_PORGVTBL[26];
struct Player : Actor {

	//跨服传送
	void transferServer(std::string server, int port) {
		WBStream ws;
		ws.apply(VarUInt(server.size()));
		ws.write(server.c_str(), server.size());
		ws.apply(VarUInt(port));
		MyPkt<0x55, false> guipk{ ws.data };
		SYMCALL(VA, MSSYM_B1QE17sendNetworkPacketB1AE12ServerPlayerB2AAE15UEBAXAEAVPacketB3AAAA1Z,
			this, &guipk);
	}

	ItemStack* getSelectedItem() {
		return SYMCALL(ItemStack*, MSSYM_B1QE15getSelectedItemB1AA6PlayerB2AAE17QEBAAEBVItemStackB2AAA2XZ, this);
	}

	bool addItem(string name, string cout, string dataid = "0") {
		runcmd("give " + this->getRealNameTag() + " " + name + " " + cout + " " + dataid);
		return true;
	}

	// 取uuid
	MCUUID* getUuid() {				// IDA ServerNetworkHandler::_createNewPlayer
		return (MCUUID*)((char*)this + 3192);
	}

	// 根据地图信息获取玩家xuid
	std::string& getXuid(VA level) {
		return SYMCALL(std::string&, MSSYM_MD5_337bfad553c289ba4656ac43dcb60748,
			level, (char*)this + 3192);
	}




	std::string getRealNameTag() {
		std::string name = this->getNameTag();
		if (name.find(' ') != name.npos) {
			return "\"" + name + "\"";
		}
		else {
			return name;
		}
	}

	void sendMsg(std::string msg) {
		msg = replace_all_distinct(msg, "\\", "\\\\");
		msg = replace_all_distinct(msg, "\"", "\\\"");
		//msg = stringToUTF8(msg);
		runcmd("tellraw " + this->getRealNameTag() + " { \"rawtext\": [{\"text\": \"" + msg + "\"}]}");
	}



	//以玩家权限执行命令 - 参考 BDX

	bool runcmdAs(string cmd) {
		void** filler[5];
		SYMCALL(void,
			MSSYM_B2QQE200PlayerCommandOriginB2AAA4QEAAB1AE10AEAVPlayerB3AAAA1Z,
			filler, this);
		if (FAKE_PORGVTBL[1] == NULL) {
			memcpy(FAKE_PORGVTBL, ((void**)filler[0]) - 1, sizeof(FAKE_PORGVTBL));
			FAKE_PORGVTBL[1] = (void*)dummy;
		}
		filler[0] = FAKE_PORGVTBL + 1;
		return MinecraftCommands::_runcmd(filler, cmd, 4, 1);
	}

	// 重设服务器玩家名
	void reName(std::string name) {
		SYMCALL(void, MSSYM_B1QA7setNameB1AA6PlayerB2AAA9UEAAXAEBVB2QDA5basicB1UA6stringB1AA2DUB2QDA4charB1UA6traitsB1AA1DB1AA3stdB2AAA1VB2QDA9allocatorB1AA1DB1AA12B2AAA3stdB3AAAA1Z,
			this, name);
	}
};


struct ContainerItemStack
	:ItemStack {

};

struct ContainerManagerModel {
	// 取开容者
	Player* getPlayer() {				// IDA ContainerManagerModel::ContainerManagerModel
		return *reinterpret_cast<Player**>(reinterpret_cast<VA>(this) + 8);
	}
};
struct LevelContainerManagerModel
	:ContainerManagerModel {
};

struct TextPacket {
	// 取输入文本
	std::string toString() {			// IDA ServerNetworkHandler::handle
		std::string str = std::string(*(std::string*)((VA)this + 80));
		return str;
	}
};

struct CommandRequestPacket {
	// 取命令文本
	std::string toString() {			// IDA ServerNetworkHandler::handle
		std::string str = std::string(*(std::string*)((VA)this + 40));
		return str;
	}
};

struct ModalFormResponsePacket {
	// 取发起表单ID
	UINT getFormId() {
		return *(UINT*)((VA)this + 40);
	}
	// 取选择序号
	std::string getSelectStr() {
		std::string x = *(std::string*)((VA)this + 48);
		int l = x.length();
		if (x.c_str()[l - 1] == '\n') {
			return l > 1 ? x.substr(0, l - 1) :
				x;
		}
		return x;
	}
};

#pragma once
#include "Prebuild.h"


#include "GUI/varint.h"
#include "GUI/Bstream.h"
#include "GUI/myPacket.h"

static VA p_spscqueue;
// ִ�к��ָ��
static bool runcmd(std::string cmd) {
	if (p_spscqueue != 0)
		return SYMCALL(bool, MSSYM_MD5_b5c9e566146b3136e6fb37f0c080d91e, p_spscqueue, cmd);
	return false;
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

// ��������ṹ��
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
	// ��ȡ������
	auto getFullName() const {				// IDA BlockLegacy::~BlockLegacy
		return (std::string&) * (__int64*)((__int64)this + 104);
	}
	// ��ȡ����ID��
	auto getBlockItemID() const {
		return SYMCALL(short,
			MSSYM_B1QE14getBlockItemIdB1AE11BlockLegacyB2AAA7QEBAFXZ,
			this);
	}
};

struct BlockPos {
	// ��ȡ��������ͷ
	BPos3* getPosition() const {
		return reinterpret_cast<BPos3*>(reinterpret_cast<VA>(this));
	}
};

struct Block {
	// ��ȡԴ
	const BlockLegacy* getLegacyBlock() const {
		return SYMCALL(const BlockLegacy*,
			MSSYM_B1QE14getLegacyBlockB1AA5BlockB2AAE19QEBAAEBVBlockLegacyB2AAA2XZ,
			this);
	}
};

struct BlockActor {
	// ȡ����
	Block* getBlock() {
		return *reinterpret_cast<Block**>(reinterpret_cast<VA>(this) + 16);
	}
	// ȡ����λ��
	BlockPos* getPosition() {				// IDA BlockActor::BlockActor
		return reinterpret_cast<BlockPos*>(reinterpret_cast<VA>(this) + 44);
	}
};

struct BlockSource {
	// ȡ����
	Block* getBlock(const BlockPos* blkpos) {
		return SYMCALL(Block*,
			MSSYM_B1QA8getBlockB1AE11BlockSourceB2AAE13QEBAAEBVBlockB2AAE12AEBVBlockPosB3AAAA1Z,
			this, blkpos);
	}
};

struct MCUUID {
	// ȡuuid�ַ���
	std::string toString() {
		std::string s;
		SYMCALL(std::string&, MSSYM_MD5_40e8abf6eb08f7ee446159cdd0a7f283, this, &s);
		return s;
	}
};

// �������ṹ��
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

struct Actor {
	// ȡ����Դ
	BlockSource* getRegion() {
		return *reinterpret_cast<BlockSource**>(reinterpret_cast<VA>(this) + 414 * sizeof(void*));
	}

	// ��ȡ����������Ϣ
	std::string getNameTag() {
		return SYMCALL(std::string&,
			MSSYM_MD5_7044ab83168b0fd345329e6566fd47fd,
			this);
		//return ;
	}

	// ��ȡ���ﵱǰ����ά��ID
	int getDimensionId() {
		int dimensionId;
		SYMCALL(int&,
			MSSYM_B1QE14getDimensionIdB1AA5ActorB2AAA4UEBAB1QA2AVB2QDE11AutomaticIDB1AE10VDimensionB2AAA1HB2AAA2XZ,
			this, &dimensionId);
		return dimensionId;
	}

	// �Ƿ�����
	const BYTE isStand() {				// IDA MovePlayerPacket::MovePlayerPacket
		return *reinterpret_cast<BYTE*>(reinterpret_cast<VA>(this) + 376);
	}

	// ��ȡ���ﵱǰ��������
	Vec3* getPos() {
		return SYMCALL(Vec3*,
			MSSYM_B1QA6getPosB1AA5ActorB2AAE12UEBAAEBVVec3B2AAA2XZ, this);
	}

	// ��ȡ��������
	std::string getTypeName() {
		std::string actor_typename;
		SYMCALL(std::string&,
			MSSYM_MD5_01064f7d893d9f9ef50acf1f931d1d79,
			&actor_typename, this);
		return actor_typename;
	}

	// ��ȡʵ������
	int getEntityTypeId() {
		return SYMCALL(int,
			MSSYM_B1QE15getEntityTypeIdB1AA5ActorB2AAA4UEBAB1QE12AW4ActorTypeB2AAA2XZ,
			this);
		//		if (t == 1)		// δ֪���ͣ����������
		//			return 319;
	}

	// ��ȡʵ������
	std::string getEntityTypeName() {
		std::string en_name;
		SYMCALL(std::string&,
			MSSYM_MD5_af48b8a1869a49a3fb9a4c12f48d5a68,
			&en_name, getEntityTypeId());
		return en_name;
	}
};
struct Mob : Actor {
};
struct Player : Actor {

	//�������
	void transferServer(std::string server, int port) {
		WBStream ws;
		ws.apply(VarUInt(server.size()));
		ws.write(server.c_str(), server.size());
		ws.apply(VarUInt(port));
		MyPkt<0x55, false> guipk{ ws.data };
		SYMCALL(VA, MSSYM_B1QE17sendNetworkPacketB1AE12ServerPlayerB2AAE15UEBAXAEAVPacketB3AAAA1Z,
			this, &guipk);
	}

	// ȡuuid
	MCUUID* getUuid() {				// IDA ServerNetworkHandler::_createNewPlayer
		return (MCUUID*)((char*)this + 3192);
	}

	// ���ݵ�ͼ��Ϣ��ȡ���xuid
	std::string& getXuid(VA level) {
		return SYMCALL(std::string&, MSSYM_MD5_337bfad553c289ba4656ac43dcb60748,
			level, (char*)this + 3192);
	}


	std::string static stringToUTF8(const std::string& str)
	{
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

		wchar_t* pwBuf = new wchar_t[nwLen + 1];//һ��Ҫ��1����Ȼ�����β�� 
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
		msg = stringToUTF8(msg);
		runcmd("tellraw " + this->getRealNameTag() + " { \"rawtext\": [{\"text\": \"" + msg + "\"}]}");		
	}


	// ��������������
	void reName(std::string name) {
		SYMCALL(void, MSSYM_B1QA7setNameB1AA6PlayerB2AAA9UEAAXAEBVB2QDA5basicB1UA6stringB1AA2DUB2QDA4charB1UA6traitsB1AA1DB1AA3stdB2AAA1VB2QDA9allocatorB1AA1DB1AA12B2AAA3stdB3AAAA1Z,
			this, name);
	}
};

struct ItemStack {
	// ȡ��ƷID
	short getId() {
		return SYMCALL(short,
			MSSYM_B1QA5getIdB1AE13ItemStackBaseB2AAA7QEBAFXZ,
			this);
	}
	// ȡ��Ʒ����ֵ
	short getAuxValue() {
		return SYMCALL(short,
			MSSYM_B1QE11getAuxValueB1AE13ItemStackBaseB2AAA7QEBAFXZ,
			this);
	}
	// ȡ��Ʒ����
	std::string getName() {
		std::string str;
		SYMCALL(__int64,
			MSSYM_MD5_6d581a35d7ad70fd364b60c3ebe93394,
			this, &str);
		return str;
	}
	// ȡ����������
	int getStackSize() {
		return SYMCALL(int,
			MSSYM_B1QA8getCountB1AE18ContainerItemStackB2AAA7QEBAHXZ,
			this);
	}
	// �ж��Ƿ������
	bool isNull() {
		return SYMCALL(bool,
			MSSYM_B1QA6isNullB1AE13ItemStackBaseB2AAA4QEBAB1UA3NXZ,
			this);
	}
};
struct ContainerItemStack
	:ItemStack {

};

struct ContainerManagerModel {
	// ȡ������
	Player* getPlayer() {				// IDA ContainerManagerModel::ContainerManagerModel
		return *reinterpret_cast<Player**>(reinterpret_cast<VA>(this) + 8);
	}
};
struct LevelContainerManagerModel
	:ContainerManagerModel {
};

struct TextPacket {
	// ȡ�����ı�
	std::string toString() {			// IDA ServerNetworkHandler::handle
		std::string str = std::string(*(std::string*)((VA)this + 80));
		return str;
	}
};

struct CommandRequestPacket {
	// ȡ�����ı�
	std::string toString() {			// IDA ServerNetworkHandler::handle
		std::string str = std::string(*(std::string*)((VA)this + 40));
		return str;
	}
};

struct ModalFormResponsePacket {
	// ȡ�����ID
	UINT getFormId() {
		return *(UINT*)((VA)this + 40);
	}
	// ȡѡ�����
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
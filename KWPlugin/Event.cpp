#pragma once
#include "Prebuild.h" //Prebuild Header
#include "BDSAPI.hpp"
#include "Land.cpp"

using namespace std;
class Event {
public:
	bool static canSpawn(Mob* mob) {
		if (!Land::canLandSpawn(Land::PlayerChunckId(mob->getPos()), mob->getEntityTypeId())) {
			return false;
		}
		return true;
	}

	bool static ActorMove(Actor* actor, Vec3* pos) {
		if (actor->hasTag("NPC")) {
			return false;
		}
	}

	//建议不要多次调用! 此方法废弃,待处置
	bool static checkItemEnchant(ItemStack* item) {
		return true;
		if (item->isEnchanted()) {
			
		}

		return true;
	}

	bool static checkItemEnchantLevel(ItemStack* item, int type, int level) {
		//cout << "Checking Enchaned item " << item->getName() << " type: " << type << " level: " << level << endl;

		switch (type)
		{
		case 8://水下速掘
		case 16://精准采集
		case 21://火失
		case 26://经验修补
		case 27://绑定
		case 28://消失
		case 32://引雷
		case 33://多重射击
			if (level > 1) return false;
			break;
		case 12://击退
		case 13://火焰附加
		case 20://冲击
		case 25://冰霜行者
			if (level > 2) return false;
			break;
		case 5://荆棘
		case 6://水下呼吸
		case 7://深海探索者
		case 14://抢夺
		case 17://耐久
		case 18://时运
		case 23://海之眷顾
		case 24://钓饵
		case 30://激流
		case 31://忠诚
		case 35://快速装填
		case 36://灵魂疾行
			if (level > 3) return false;
			break;
		case 0://保护
		case 1://火焰保护
		case 2://掉落保护
		case 3://爆炸保护
		case 4://弹射物保护
		case 34://穿透
			if (level > 4) return false;
			break;
		case 9://锋利
		case 10://亡灵杀手
		case 11://节支杀手
		case 15://效率
		case 19://力量
		case 29://穿刺
			if (level > 5) return false;
			break;
		default:
			return true;
			break;
		}
		return true;
	}
};
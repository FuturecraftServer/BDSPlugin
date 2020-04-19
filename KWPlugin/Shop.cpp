#pragma once
#include "Prebuild.h"
#include "CConfig.cpp"

struct ShopItem {
	int id;
	int dataid;
	string uniqueid;
	int buyprice;
	int sellprice;
	int cont;

	ShopItem(int id, int dataid) {
		this->id = id;
		this->dataid = dataid;
		this->uniqueid = std::to_string(id) + ":" + std::to_string(dataid);
		this->buyprice = CConfig::GetValueInt("Shop", "buy", this->uniqueid, 0);
		this->sellprice = CConfig::GetValueInt("Shop", "sell", this->uniqueid, 0);
		this->cont = CConfig::GetValueInt("Shop", "count", this->uniqueid, 0);
	}

	ShopItem(string id, string dataid) {
		this->id = atoi(id.c_str());
		this->dataid = atoi(dataid.c_str());
		this->uniqueid = id + ":" + dataid;
		this->buyprice = CConfig::GetValueInt("Shop", "buy", this->uniqueid, 0);
		this->sellprice = CConfig::GetValueInt("Shop", "sell", this->uniqueid, 0);
		this->cont = CConfig::GetValueInt("Shop", "count", this->uniqueid, 0);
	}
	ShopItem(string uniqueid) {
		int index = uniqueid.find(":");
		this->id = atoi(uniqueid.assign(uniqueid.c_str(), index).c_str());
		this->dataid = atoi(uniqueid.substr(index).c_str());
		this->uniqueid = uniqueid;
		this->buyprice = CConfig::GetValueInt("Shop", "buy", uniqueid, 0);
		this->sellprice = CConfig::GetValueInt("Shop", "sell", uniqueid, 0);
		this->cont = CConfig::GetValueInt("Shop", "count", uniqueid, 0);
	}

	void SaveItem() {
		CConfig::SetValueString("Shop", "buy", this->uniqueid, std::to_string(this->buyprice));
		CConfig::SetValueString("Shop", "sell", this->uniqueid, std::to_string(this->sellprice));
		CConfig::SetValueString("Shop", "cont", this->uniqueid, std::to_string(this->cont));
	}
};

class Shop {
	void static SetPrice(int id, int dataid, int buy, int sell, int cont) {
		ShopItem item = ShopItem(id, dataid);
		item.buyprice = buy;
		item.sellprice = sell;
		item.cont = cont;
		item.SaveItem();
	}

	vector<ShopItem> static GetShopSellItem() {
		vector<string> keys = CConfig::GetSectionKeys("Shop", "sell");
		vector<ShopItem> ret;
		for (vector<string>::const_iterator iter = keys.cbegin(); iter != keys.cend(); iter++) {
			ret.push_back(ShopItem((*iter)));
		}
		return ret;
	}

	vector<ShopItem> static GetShopbuyItem() {
		vector<string> keys = CConfig::GetSectionKeys("Shop", "buy");
		vector<ShopItem> ret;
		for (vector<string>::const_iterator iter = keys.cbegin(); iter != keys.cend(); iter++) {
			ret.push_back(ShopItem((*iter)));
		}
		return ret;
	}

	string static intToString(int v)
	{
		return std::to_string(v);
	}
};

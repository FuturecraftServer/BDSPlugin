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
	string name;
	string texture;
	string nameid;

	ShopItem(int id, int dataid) {
		this->id = id;
		this->dataid = dataid;
		this->uniqueid = std::to_string(id) + ":" + std::to_string(dataid);
		this->buyprice = CConfig::GetValueInt("Shop", "buy", this->uniqueid, 0);
		this->sellprice = CConfig::GetValueInt("Shop", "sell", this->uniqueid, 0);
		this->cont = CConfig::GetValueInt("Shop", "count", this->uniqueid, 0);
		this->name = CConfig::GetValueString("Shop", "name", this->uniqueid, "未知商品");
		this->texture = CConfig::GetValueString("Shop", "texture", this->uniqueid, "NaN");
		this->nameid = CConfig::GetValueString("Shop", "nameid", this->uniqueid, "NaN");
	}

	ShopItem(string id, string dataid) {
		this->id = atoi(id.c_str());
		this->dataid = atoi(dataid.c_str());
		this->uniqueid = id + ":" + dataid;
		this->buyprice = CConfig::GetValueInt("Shop", "buy", this->uniqueid, 0);
		this->sellprice = CConfig::GetValueInt("Shop", "sell", this->uniqueid, 0);
		this->cont = CConfig::GetValueInt("Shop", "count", this->uniqueid, 0);
		this->name = CConfig::GetValueString("Shop", "name", this->uniqueid, "未知商品");
		this->texture = CConfig::GetValueString("Shop", "texture", this->uniqueid, "NaN");
		this->nameid = CConfig::GetValueString("Shop", "nameid", this->uniqueid, "NaN");

	}
	ShopItem(string uniqueid) {
		this->uniqueid = uniqueid;
		int index = uniqueid.find(":");
		this->id = atoi(uniqueid.assign(uniqueid.c_str(), index).c_str());
		uniqueid = this->uniqueid;
		string dataid = uniqueid.substr(index + 1).c_str();
		this->dataid = atoi(dataid.c_str());
		uniqueid = this->uniqueid;
		this->buyprice = CConfig::GetValueInt("Shop", "buy", uniqueid, 0);
		this->sellprice = CConfig::GetValueInt("Shop", "sell", uniqueid, 0);
		this->cont = CConfig::GetValueInt("Shop", "count", uniqueid, 0);
		this->name = CConfig::GetValueString("Shop", "name", uniqueid, "未知商品");
		this->texture = CConfig::GetValueString("Shop", "texture", uniqueid, "NaN");
		this->nameid = CConfig::GetValueString("Shop", "nameid", uniqueid, "NaN");
	}

	void SaveItem() {
		CConfig::SetValueString("Shop", "buy", this->uniqueid, std::to_string(this->buyprice));
		CConfig::SetValueString("Shop", "sell", this->uniqueid, std::to_string(this->sellprice));
		CConfig::SetValueString("Shop", "count", this->uniqueid, std::to_string(this->cont));
		CConfig::SetValueString("Shop", "name", this->uniqueid, this->name);
		CConfig::SetValueString("Shop", "texture", this->uniqueid, this->texture);
		CConfig::SetValueString("Shop", "nameid", this->uniqueid, this->nameid);
	}
};

class Shop {
public:
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

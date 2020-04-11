#pragma once
#include "Prebuild.h" //Prebuild Header
#include <string>
#include "BDSAPI.hpp"
#include "CConfig.cpp"

using namespace std;
class RPG {
public:
	bool static isNewPlayer(string player) {
		if (CConfig::GetValueInt("RPG", player, "old", 0) == 0) {
			CConfig::SetValueString("RPG", player, "old", "1");
			return true;
		}
		else {
			return false;
		}
	}

};
#pragma once
#include "Prebuild.h"
#include "SimpleIni/SimpleIni.h"
#include <windows.h>
#include <fstream>

using namespace std;
// 获取BDS完整程序路径
static char localpath[MAX_PATH] = { 0 };
static std::string getLocalPath() {
	if (!localpath[0]) {
		GetModuleFileNameA(NULL, localpath, _countof(localpath));
		for (int l = strlen(localpath); l >= 0; l--) {
			if (localpath[l] == '\\') {
				localpath[l] = localpath[l + 1] = localpath[l + 2] = 0;
				break;
			}
		}
	}
	return std::string(localpath) + "\\";
}

class CConfig {
public:
	string static GetValueString(string filepath, string section, string key, string defvalue = "") {
		filepath = getLocalPath() + "Plugin\\" + filepath + ".ini";
		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(filepath.c_str());
		return ini.GetValue(section.c_str(), key.c_str(), defvalue.c_str());
	}

	int static GetValueInt(string filepath, string section, string key, int defvalue = 0) {
		filepath = getLocalPath() + "Plugin\\" + filepath + ".ini";
		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(filepath.c_str());
		return ini.GetLongValue(section.c_str(), key.c_str(), defvalue);
	}

	bool static SetValueString(string filepath, string section, string key, string value = "") {
		filepath = getLocalPath() + "Plugin\\" + filepath + ".ini";
		//cout << u8"File Path: " << filepath << endl << u8"Section: " << section << endl << "Key: " << key << endl << "Value: " << value << endl;
		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(filepath.c_str());
		ini.SetValue(section.c_str(), key.c_str(), value.c_str());
		ini.SaveFile(filepath.c_str());
		return true;
	}

	string static GetPluginPath() {
		return getLocalPath() + "Plugin\\";
	}
};
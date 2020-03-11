#include "Prebuild.h"
#include <string>
#include <stringapiset.h>
#include <WinBase.h>
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
		LPSTR value = new char[6];
		filepath = getLocalPath() + filepath + ".ini";
		GetPrivateProfileString(section.c_str(), key.c_str(), defvalue.c_str(), value, 6, filepath.c_str());
		return value;
	}

	int static GetValueInt(string filepath, string section, string key, int defvalue = 0) {
		filepath = getLocalPath() + filepath + ".ini";
		return GetPrivateProfileInt(section.c_str(), key.c_str(), defvalue, filepath.c_str());
	}

	bool static SetValueString(string filepath, string section, string key, string value = "") {
		filepath = getLocalPath() + filepath + ".ini";
		return WritePrivateProfileString(section.c_str(), key.c_str(), value.c_str(), filepath.c_str());
	}
};
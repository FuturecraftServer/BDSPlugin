#pragma once

#include <string>
#include <vector>
#include "Prebuild.h"

extern unsigned sendForm(VA , std::string);

extern bool destroyForm(unsigned);

//extern std::string createSimpleFormString(std::string, std::string, Json::Value&);

extern std::string createModalFormString(std::string, std::string, std::string, std::string);

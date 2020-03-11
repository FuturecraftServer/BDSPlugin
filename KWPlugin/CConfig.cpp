/********************************************************
Copyright (C),  2016-2018,
FileName:		CConfig
Author:        	woniu201
Created:       	2018/11/16
Description:	纯C++实现配置文件的操作
********************************************************/
#include "Prebuild.h"
#include "CConfig.h"


CConfig::CConfig()
{

}

CConfig::~CConfig()
{

}

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

/************************************
@ Brief:		设置配置文件路径
@ Author:		woniu201
@ Created:		2018/12/21
@ Return:
************************************/
void CConfig::SetFilePath(const string& filePath)
{
	m_filePath = getLocalPath() + "Config\\" + filePath + ".ini";
}

/************************************
@ Brief:		读取配置文件路径
@ Author:		woniu201
@ Created:		2018/12/21
@ Return:
************************************/
string CConfig::GetFilePath()
{
	return this->m_filePath;
}

/************************************
@ Brief:		打开配置文件
@ Author:		woniu201
@ Created:		2018/12/21
@ Return:
************************************/
bool CConfig::OpenFile()
{
	if (true == m_fin.is_open())
	{
		m_fin.close();
	}

	m_fin.open(m_filePath.c_str());

	if (!m_fin.is_open())
	{
		m_error = "open file fail:" + m_filePath;
		return false;
	}
	return true;
}

/************************************
@ Brief:		找节名
@ Author:		woniu201
@ Created:		2018/12/21
@ Return:
************************************/
bool CConfig::FindSection(const string& sectionName)
{
	if (-1 != m_content.find('['))
	{
		string sTemp = m_content.substr(m_content.find('[') + 1, m_content.find(']') - m_content.find('[') - 1);
		if (0 == strcmp(sTemp.c_str(), sectionName.c_str()))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

/************************************
@ Brief:		找键名
@ Author:		woniu201
@ Created:		2018/12/21
@ Return:
************************************/
bool CConfig::FindKey(const string& key)
{
	size_t iDelePlace = m_content.find((char)'//', 0);
	size_t iFindEqual = m_content.find((char)'=', 0);
	//被注释的行，或者是包含key但是已经被注视掉了，过滤
	if ((-1 != iDelePlace && iDelePlace < iFindEqual) || (-1 != iDelePlace && -1 == iFindEqual) || -1 == iFindEqual)
	{
		return false;
	}
	string sKey = m_content.substr(0, m_content.find('='));

	if (0 == strcmp(sKey.c_str(), key.c_str()))
	{
		m_value = m_content.substr(m_content.find('=') + 1, m_content.length() - m_content.find('=') - 1);
		return true;
	}
	return false;
}

/************************************
@ Brief:		读取节下Key对应的value值
@ Author:		woniu201
@ Created:		2018/12/21
@ Return:
************************************/
bool CConfig::GetValue(const string& section, const string& key, string& value, string& error)
{
	m_error = "";
	if (false == OpenFile())
	{
		error = m_error;
		return false;
	}
	char str[4096] = { 0 };
	bool bFindSection = false;
	while (m_fin.getline(str, sizeof(str)))
	{
		m_content = str;
		if (!bFindSection)
		{
			if (FindSection(section))
			{
				bFindSection = true;
			}
			else
			{
				m_error = "";
				m_error = "节点" + section + "不存在";
			}
		}
		else
		{
			if (FindKey(key))
			{
				m_fin.close();
				m_error = "";
				value = m_value;
				return true;
			}
			else
			{
				m_error = "";
				m_error = "键名" + key + "不存在";
			}
		}
		memset(str, 0, 4096);
	}
	error = m_error;
	m_fin.close();
	return false;
}

/************************************
@ Brief:		读的方式打开文件
@ Author:		woniu201
@ Created:		2018/12/21
@ Return:
************************************/
bool CConfig::OpenFileRead()
{
	m_fout.close();
	//关闭后要在清空一下，否则下次打开会出错
	m_fout.clear();
	m_fout.open(m_filePath.c_str(), ios::in);
	if (!m_fout.is_open())
	{
		m_error = "open file fail:" + m_filePath;
		return false;
	}
	return true;
}

/************************************
@ Brief:		写的方式打开文件
@ Author:		woniu201
@ Created:		2018/12/21
@ Return:
************************************/
bool CConfig::OpenFileWrite()
{
	m_fout.close();
	//关闭后要在清空一下，否则下次打开会出错
	m_fout.clear();
	m_fout.open(m_filePath.c_str(), ios::out | ios::trunc);
	if (!m_fout.is_open())
	{
		m_error = "can not open file " + m_filePath;
		return false;
	}
	return true;
}

/************************************
@ Brief:		找KEY，设置新值
@ Author:		woniu201
@ Created:		2018/12/21
@ Return:
************************************/
bool CConfig::SetValue(const string& key, const string& value)
{
	size_t iDelePlace = m_content.find((char)'//');
	size_t iFindEqual = m_content.find((char)'=');
	//被注释的行，或者是包含key但是已经被注视掉了，过滤
	if ((-1 != iDelePlace && iDelePlace < iFindEqual) || (-1 != iDelePlace && -1 == iFindEqual) || -1 == iFindEqual)
	{
		return false;
	}
	string sKey = m_content.substr(0, m_content.find('='));

	if (0 == strcmp(sKey.c_str(), key.c_str()))
	{
		m_content = m_content.substr(0, m_content.find('=') + 1) + value;
		return true;
	}

	return false;
}


/************************************
@ Brief:		修改配置文件KEY对应的value
@ Author:		woniu201
@ Created:		2018/12/21
@ Return:
************************************/
bool CConfig::ModifyValue(const string& section, const string& key, const string& value, string& error)
{
	m_error = "";
	if (false == OpenFileRead())
	{
		error = m_error;
		return false;
	}

	char str[4096] = { 0 };
	vector<string> vContent;
	bool isModify = false;
	bool isFindSection = false;
	while ((m_fout.getline(str, sizeof(str))))
	{
		m_content = str;
		if (!isFindSection)
		{
			if (FindSection(section))
			{
				isFindSection = true;
			}
			else
			{
				m_error = "";
				m_error = "节点" + section + "不存在";
			}
		}
		else
		{
			if (!isModify)
			{
				if (SetValue(key, value))
				{
					isModify = true;
				}
				else
				{
					m_error = "";
					m_error = "键名" + key + "不存在";
				}
			}
		}
		vContent.push_back(m_content);
		m_content = "";
		memset(str, 0, 4096);
	}
	error = m_error;
	WriteFile(vContent);
	m_fout.flush();
	m_fout.close();
	return isModify;
}

/************************************
@ Brief:		写文件
@ Author:		woniu201
@ Created:		2018/12/21
@ Return:
************************************/
void CConfig::WriteFile(vector<string>& vContent)
{
	if (false == OpenFileWrite())
	{
		m_fout.close();
		return;
	}
	for (size_t iIndex = 0; iIndex < vContent.size(); iIndex++)
	{
		m_fout << vContent[iIndex] << endl;
	}
	m_fout.close();
	vector<string>().swap(vContent);
}
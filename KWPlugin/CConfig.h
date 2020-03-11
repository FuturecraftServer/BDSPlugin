#ifndef _CCONFIG_H
#define _CCONFIG_H
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

class CConfig
{
public:
	CConfig();
	~CConfig();
	void	SetFilePath(const string& filePath);
	string	GetFilePath();
	bool	GetValue(const string& section, const string& key, string& value, string& error);
	bool	ModifyValue(const string& section, const string& key, const string& value, string& error);

private:
	bool	OpenFile();
	bool	FindSection(const string& sectionName);
	bool	FindKey(const string& key);
	bool	OpenFileRead();
	bool	OpenFileWrite();
	bool	SetValue(const string& key, const string& value);
	void	WriteFile(vector<string>& vContent);

	string	m_filePath;
	fstream m_fout;
	fstream m_fin;
	string	m_content;
	string  m_value;
	string	m_error;
};
#endif
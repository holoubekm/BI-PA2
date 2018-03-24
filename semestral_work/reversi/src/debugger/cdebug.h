#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <cctype>
#include <cstring>
#include <unistd.h>
#include <climits>

using namespace std;

typedef ostream& (*ostream_manipulator)(ostream&);

class CDebug
{
private:
	CDebug& Write(string out);
	static CDebug& GetInstance();
private:
	int CFile;
public:
	CDebug();
	CDebug& operator <<(const int& val);
	CDebug& operator <<(const long& val);
	CDebug& operator <<(const double& val);
	CDebug& operator <<(const char& val);
	CDebug& operator <<(const string& val);
	CDebug& operator<<(ostream_manipulator pf);
	static bool Debug(const string& out);
	static bool Error(const string& out);
};

#define debug(a) CDebug::Debug(a);
extern CDebug deb;

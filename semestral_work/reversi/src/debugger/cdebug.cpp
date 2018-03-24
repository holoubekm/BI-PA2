#include "cdebug.h"

CDebug deb;
CDebug::CDebug()
{
	cout << "creating new instance" << endl;
	const int err = -1;
	CFile = posix_openpt(O_RDWR);
	if (CFile == err)
	{
		std::cerr << "Could not open pseudo terminal.\n";
	}

	char* ptname = ptsname(CFile);
	if (!ptname)
	{
		std::cerr << "Could not get pseudo terminal device name.\n";
		close(CFile);
	}

	if (unlockpt(CFile) == err)
	{
		std::cerr << "Could not get pseudo terminal device name.\n";
		close(CFile);
	}
	std::ostringstream oss;
	oss << "xterm  +cm -S" << (strrchr(ptname, '/')+1) << "/" << CFile << " &";
	system(oss.str().c_str());

	int xterm_fd = open(ptname,O_RDWR);
	char c;
	do read(xterm_fd, &c, 1); while (c!='\n');
}


CDebug& CDebug::Write(string out)
{
	write(CFile, out.c_str(), out.size());
	return *this;
}

CDebug& CDebug::GetInstance()
{
	return deb;
}

bool CDebug::Debug(const string& out)
{
	deb << "Debug: " << out << endl;
	return true;
}

bool CDebug::Error(const string& out)
{
	deb << "Error: " << out << endl;
	return true;
}

CDebug& CDebug::operator<<(ostream_manipulator val)
{
	ostringstream os;
	os << val;
	return Write(os.str());
}

CDebug& CDebug::operator <<(const long& val)
{
	ostringstream os;
	os << val;
	return Write(os.str());
}

CDebug& CDebug::operator <<(const char& val)
{
	ostringstream os;
	os << val;
	return Write(os.str());
}

CDebug& CDebug::operator <<(const int& val)
{
	ostringstream os;
	os << val;
	return Write(os.str());
}

CDebug& CDebug::operator <<(const double& val)
{
	ostringstream os;
	os << val;
	return Write(os.str());
}

CDebug& CDebug::operator <<(const string& val)
{
	return Write(val);
}

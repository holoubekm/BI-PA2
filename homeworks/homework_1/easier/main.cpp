#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#endif /* __PROGTEST__ */

template<class T>
struct sort_subjects : std::binary_function<T, T, bool>
{
	inline bool operator()(const T& lhs, const T& rhs)
	{
		return lhs.first < rhs.first;
	}
};

void Error(int line)
{
	#ifndef __PROGTEST__
		std::cout << "Error at line #: " << line << std::endl;
	#endif //__PROGTEST__
}

class Parser
{
private:
	std::vector<std::string> added;
	std::map<std::string, std::set<std::string> > subjects;
	const char* outFile;

public:
	inline Parser(const char* _outFile) : outFile(_outFile) {};
	bool ParseLine(std::string line);
	bool PrintOutput();
private:
	bool AddSubject(std::string subject, std::string teacher);
	std::string TrimStart(std::string line);
	std::string TrimEnd(std::string line);

};

bool Parser::AddSubject(std::string subject, std::string teacher)
{
	//Check for duplicated teachers
	if (find(added.begin(), added.end(), teacher) != added.end())
	{
		Error(__LINE__);
		return false;
	}

	//Get teachers, in the case of non existent, the default one is created
	std::set<std::string>& teachers = subjects[subject];
	//Teacher can't teach one subject multiple times
	if (teachers.find(teacher) != teachers.end())
	{
		Error(__LINE__);
		return false;
	}
	else
	{
		teachers.insert(teacher);
	}


	return true;
}

std::string Parser::TrimStart(std::string line)
{
	size_t pos = 0;
	size_t size = line.size();
	if (size > 0)
	{
		while (pos < size && isspace(line[pos]) && line[pos] != ',' && line[pos] != ':') { pos++; }
		return line.substr(pos, size - pos);
	}

	return std::string();
}

std::string Parser::TrimEnd(std::string line)
{
	size_t size = line.size();
	long pos = (long)size - 1;
	if (size > 0)
	{
		while (pos >= 0 && isspace(line[pos]) && line[pos] != ',' && line[pos] != ':') { pos--; }
		if (pos == -1)
		{
			return std::string();
		}
		return line.substr(0, pos + 1);
	}
	return std::string();
}

bool Parser::ParseLine(std::string line)
{
	if (line.size() == 0)
	{
		return true;
	}

	size_t pos = line.find_first_of('#');
	if (pos == 0)
	{
		return true;
	}
	else if(pos != std::string::npos)
	{
		Error(__LINE__);
		return false;
	}

	line = TrimStart(line);

	if (line.size() == 0)
	{
		return true;
	}

	//Check colon existence and teacher non existence
	pos = line.find_first_of(':');
	if (pos == std::string::npos || pos == 0)
	{
		Error(__LINE__);
		return false;
	}

	//Comma that precede colon
	size_t colPos = line.find_first_of(',');
	if (colPos >= 0 && colPos < pos)
	{
		Error(__LINE__);
		return false;
	}
	std::string name = line.substr(0, pos);
	name = TrimEnd(name);
	for (unsigned int x = 0; x < name.size(); x++)
	{
		if (!isalnum(name[x]))
		{
			Error(__LINE__);
			return false;
		}
	}

	if (name.size() == 0)
	{
		Error(__LINE__);
		return false;
	}

	line = line.substr(pos + 1, line.size() - pos);
	line = TrimStart(line);
	line = TrimEnd(line);
	
	if (line.size() > 0 && line[line.size() - 1] == ',')
	{
		Error(__LINE__);
		return false;
	}

	while ((pos = line.find_first_of(',')) != std::string::npos)
	{
		//Two or more commas in a row
		if (pos == 0)
		{
			Error(__LINE__);
			return false;
		}

		std::string subject = line.substr(0, pos);
		subject = TrimEnd(subject);
		for (unsigned int x = 0; x < subject.size(); x++)
		{
			if (!isalnum(subject[x]))
			{
				Error(__LINE__);
				return false;
			}
		}

		if (!AddSubject(subject, name))
		{
			Error(__LINE__);
			return false;
		}

		line = line.substr(pos + 1, line.size() - pos);
		line = TrimStart(line);
	}

	std::string subject = line;
	subject = TrimEnd(subject);
	for (unsigned int x = 0; x < subject.size(); x++)
	{
		if (!isalnum(subject[x]))
		{
			Error(__LINE__);
			return false;
		}
	}

	if (subject.size() > 0)
	{
		if (!AddSubject(subject, name))
		{
			Error(__LINE__);
			return false;
		}
	}

	added.push_back(name);
	return true;
}

bool Parser::PrintOutput()
{
	std::ofstream output(outFile, std::ofstream::out);
	if (!output.is_open())
	{
		Error(__LINE__);
		return false;
	}

	std::vector<std::pair<std::string, std::set<std::string> > > subjs(subjects.begin(), subjects.end());
	//Alphabetically sort subjects
	sort(subjs.begin(), subjs.end(), sort_subjects<std::pair<std::string, std::set<std::string> > >());
	std::vector<std::pair<std::string, std::set<std::string> > >::iterator it;

	//For each subject print corresponding teachers
	for (it = subjs.begin(); it != subjs.end(); ++it)
	{
		output << it->first << ": ";
		//std::cout << it->first << ": ";
		std::set<std::string> teachers = it->second;

		int count = teachers.size();
		int index = 0;
		std::set<std::string>::iterator it2;

		//Print teachers separated with collons
		for (it2 = teachers.begin(); it2 != teachers.end(); ++it2)
		{
			output << *it2;
			//std::cout << *it2;
			if (index != count - 1)
			{
				output << ", ";
				//std::cout << ", ";
			}
			index++;
		}
		//std::cout << std::endl;
		output << std::endl;
	}

	output.close();

	return true;
}


bool transform(const char * inFile, const char * outFile)
{
	if (inFile == NULL || outFile == NULL)
	{
		Error(__LINE__);
		return false;
	}

	std::ifstream test(outFile, std::ifstream::in);
	if(test.is_open())
	{
		Error(__LINE__);
		test.close();
		return false;
	}
	
	std::ifstream input(inFile, std::ifstream::in);
	if (!input.is_open())
	{
		Error(__LINE__);
		return false;
	}

	std::string line;
	int count = 0;

	Parser parser(outFile);
	while (getline(input, line))
	{
		if (!parser.ParseLine(line))
		{
			input.close();
			Error(__LINE__);
			return false;
		}
		count++;
	}
	input.close();

	if (count == 0)
	{
		return true;
	}

	return parser.PrintOutput();
}


#ifndef __PROGTEST__
void assert(bool in, bool supposed)
{
	if (in != supposed)
	{
		std::cout << "Assert error" << std::endl;
	}
}

int main(int argc, char * argv[])
{
	//std::cout << transform("tests/in_2.txt", "output.txt") << std::endl;
	
	//std::cout << transform(NULL, NULL) << std::endl;
	Parser parser("sadfasd");
	assert(parser.ParseLine(std::string("lada:ahoj,ne,ahojk")), true);
	assert(parser.ParseLine(std::string("2lada:ahojg,ahodj,ashoj")), true);
	assert(parser.ParseLine(std::string(",3lada:aho,ahoj,ah")), false);
	assert(parser.ParseLine(std::string(",5la da:aoj,ahoj,aho")), false);
	assert(parser.ParseLine(std::string("3la,da:ahoj,ahoj,ahoj")), false);
	assert(parser.ParseLine(std::string("4lada:ahoj,,ahoj,ahoj")), false);
	assert(parser.ParseLine(std::string("5lada:ahoj,ah oj,ahoj")), false);
	assert(parser.ParseLine(std::string("6lada,:ahoj,ahoj,ahoj")), false);
	assert(parser.ParseLine(std::string("7lada:,ahoj,:ahoj,ahoj")), false);
	assert(parser.ParseLine(std::string("8la,da:ahoj,ahoj,ahoj,")), false);
	assert(parser.ParseLine(std::string("lada:ahoj,:ahoj,ahoj")), false);
	assert(parser.ParseLine(std::string("lada:")), true);
	assert(parser.ParseLine(std::string("8la:,")), false);
	assert(parser.ParseLine(std::string("#8la:,")), true);
	assert(parser.ParseLine(std::string(":8la")), false);
	assert(parser.ParseLine(std::string("lada, PA1, PA2, OSY")), false);
	assert(parser.ParseLine(std::string("lada: PA2:")), false);
	assert(parser.ParseLine(std::string(":lada")), false);
	assert(parser.ParseLine(std::string("lada,:")), false);
	assert(parser.ParseLine(std::string(",")), false);
	assert(parser.ParseLine(std::string(":")), false);
	assert(parser.ParseLine(std::string("lada,:")), false);
	assert(parser.ParseLine(std::string(",:,")), false);
	assert(parser.ParseLine(std::string(": :")), false);
	assert(parser.ParseLine(std::string("")), true);
	assert(parser.ParseLine(std::string("la da")), false);
	assert(parser.ParseLine(std::string("lada: pa 2")), false);
	assert(parser.ParseLine(std::string("lada")), false);
	assert(parser.ParseLine(std::string("la da: PA2, OSY")), false);
	assert(parser.ParseLine(std::string("lada: PA2,")), false);
	assert(parser.ParseLine(std::string("lada: PA2,,OSY")), false);
	assert(parser.ParseLine(std::string("ladik: PA3")), true);
	assert(parser.ParseLine(std::string("#koment")), true);
	assert(parser.ParseLine(std::string("# koment")), true);
	assert(parser.ParseLine(std::string("  ")), true);
	assert(parser.ParseLine(std::string("    lada: asdfa,asdf,")), false);
	assert(parser.ParseLine(std::string("#,,,,,:")), true);
	assert(parser.ParseLine(std::string(":,..")), false);
	assert(parser.ParseLine(std::string("?asdf>:345,asdf")), false);
	assert(parser.ParseLine(std::string("sdf:45,34,")), false);
	assert(parser.ParseLine(std::string("la da: PA2")), false);
	assert(parser.ParseLine(std::string("lada: PA1 PA2")), false);
	assert(parser.ParseLine(std::string(",")), false);
	assert(parser.ParseLine(std::string(":")), false);
	assert(parser.ParseLine(std::string("lada: PA1,,PA2")), false);
	assert(parser.ParseLine(std::string("lada: PA1,")), false);
	assert(parser.ParseLine(std::string("lada PA1")), false);



	//std::cout << transform(NULL, NULL) << std::endl;
#ifdef _WIN32
	system("pause");
#endif //_WIN32
	return 0;
}
#endif /* __PROGTEST__ */
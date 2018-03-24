#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */

#ifndef __PROGTEST__
    #define assert(a, b) if((a) != (b)) { cout << "Shit happend at: " << __LINE__ << endl; }
    #define retFalse() cout << "Return false at: " << __LINE__ << endl;
#else
    #define retFalse()
    #define assert(a, b)
#endif

typedef bool(*comp)(const string&, const class Person*);

class Person
{
public:
    string mStringID;
    long mSumIncome;
    long mSumExpense;
public:
    Person(string& stringID) : mStringID(stringID) { mSumIncome = 0; mSumExpense = 0; }
    inline void Income(int amount) { mSumIncome += amount; }
    inline void Expense(int amount) { mSumExpense += amount; }
    static inline bool LesserID(const string& a, const Person* b) { return a < b->mStringID; }
    static inline bool GreaterID(const string& a, const Person* b) { return a > b->mStringID; }
    static inline bool EqualID(const string& a, const Person* b) { return a == b->mStringID; }
};


class CTaxRegister
{
private:
    vector<Person*> people;
    const string delimiter;
private:
    long Find(string id, bool& exactly, comp less, comp great, comp equal) const;
public:
    CTaxRegister(void) : delimiter(":-|-:") {}
    ~CTaxRegister(void);
    bool Birth(const string& name, const string& addr);
    bool Death(const string& name, const string& addr);
    bool Income(const string& name, const string& addr, int amount);
    bool Expense(const string& name, const string& addr, int amount);
    bool Audit(const string& name, const string& addr, int &sumIncome, int &sumExpense ) const; 
};

long CTaxRegister::Find(string id, bool& exactly, comp less, comp great, comp equal) const
{
    long start = 0;
    long middle = 0;
    long end = people.size() - 1;

    if(end == -1)
    {
        exactly = false;
        return end;
    }

    while (start <= end)
    {
        middle = (start + end) / 2;
        //cout << middle << endl;
        if (less(id, people[middle]))
        {
           end = middle - 1;
        }
        else if (great(id, people[middle]))
        {
            start = middle + 1;
        }
        else
        {
            break;
        }
    }

    if (equal(id, people[middle]))
        exactly = true;
    else
    {
        exactly = false;
        if(great(id, people[middle]))
            middle++;
    }

    return middle;
}

bool CTaxRegister::Birth(const string& name, const string& addr)
{
    if(name.empty() || addr.empty())
    {
        retFalse();
        return false;
    }

    string id = name + delimiter + addr;
    bool exactly = false;
    long pos = Find(id, exactly, Person::LesserID, Person::GreaterID, Person::EqualID);
    if(exactly)
    {   
        //cout << people[pos]->mStringID << " " << name << "::" <<  addr << endl;
        retFalse();
        return false;
    }


    Person* kiddie = new Person(id);
    if(pos == (long)people.size() || pos == -1)
    {
        people.push_back(kiddie);
    }
    else
    {
        people.insert(people.begin() + pos, kiddie);
    }

    return true;
}

bool CTaxRegister::Income(const string& name, const string& addr, int amount)
{
    if(name.empty() || addr.empty())
    {
        retFalse();
        return false;
    }

    string id = name + delimiter + addr;
    bool exactly = false;
    long pos = Find(id, exactly, Person::LesserID, Person::GreaterID, Person::EqualID);
    if(!exactly)
    {
        retFalse();
        return false;
    }

    Person*& ritchie = people.at(pos);
    ritchie->Income(amount);

    return true;
}

bool CTaxRegister::Expense(const string& name, const string& addr, int amount)
{
    if(name.empty() || addr.empty())
    {
        retFalse();
        return false;
    }

    string id = name + delimiter + addr;
    bool exactly = false;
    long pos = Find(id, exactly, Person::LesserID, Person::GreaterID, Person::EqualID);
    if(!exactly)
    {
        retFalse();
        return false;
    }

    Person*& poorman = people.at(pos);
    poorman->Expense(amount);
    return true;
}

bool CTaxRegister::Audit(const string& name, const string& addr, int &sumIncome, int &sumExpense ) const
{
    if(name.empty() || addr.empty())
    {
        retFalse();
        return false;
    }

    string id = name + delimiter + addr;
    bool exactly = false;
    long pos = Find(id, exactly, Person::LesserID, Person::GreaterID, Person::EqualID);
    if(!exactly)
    {
        retFalse();
        return false;
    }

    const Person* scared = people.at(pos);
    sumIncome = scared->mSumIncome;
    sumExpense = scared->mSumExpense;
    return true;
}

bool CTaxRegister::Death(const string& name, const string& addr)
{
    if(name.empty() || addr.empty())
    {
        retFalse();
        return false;
    }

    string id = name + delimiter + addr;
    bool exactly = false;
    long pos = Find(id, exactly, Person::LesserID, Person::GreaterID, Person::EqualID);
    if(!exactly)
    {
        retFalse();
        return false;
    }

    Person* oldie = people.at(pos);
    people.erase(people.begin() + pos);
    delete oldie;
    return true;
}

CTaxRegister::~CTaxRegister(void)
{
    for_each(people.begin(), people.end(), [](Person*& corpse){ delete corpse; });
    people.clear();
}

/*
CTaxRegister::~CTaxRegister(void)
{int x = 0;
    for_each(people.begin(), people.end(), [&x](Person*& corpse){ cout << x++ << ": " << corpse->mStringID << ": " << corpse->mSumExpense << ", " << corpse->mSumIncome << endl; delete corpse; });
    people.clear();
}*/

#ifndef __PROGTEST__

/*
void assert(bool out, bool ref)
{
    if(out != ref)
    {
        cout << "shit happend" << endl;
    }
    else
    {
        cout << "okej" << endl;       
    }
}
*/

string generateString() 
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    string a;
    for (int i = 0; i < rand() % 1 + 2; ++i) {
        a += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return a;
}

int main(int argc, char* argv[])
{

    bool   status;
    int    sumIncome = 0, sumExpense = 0;

    srand (time(NULL));
    CTaxRegister b1;

    vector<string> name;
    vector<string> addr;

    int add = rand() % 5000 + 2000;
    int rem = rand() % 3500 + 1000;

    for(int x = 0; x < 100; x++)
    {
        name.push_back(generateString());
        addr.push_back(generateString());

        status = b1.Birth (name[x], addr[x]);
        b1.Income(name[x], addr[x], add);
        b1.Expense(name[x], addr[x], rem);
        b1.Audit(name[x], addr[x], sumIncome, sumExpense);
        cout << x << ": " << name[x] << ":" << addr[x] << " Income: " << sumIncome << ", Expense: " << sumExpense << endl;
        //b1.Death(name[x], addr[x]);
    }

    return 0;
}
#endif //__PROGTEST__                  

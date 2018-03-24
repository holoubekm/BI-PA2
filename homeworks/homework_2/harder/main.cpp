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

typedef bool(*comp)(const string&, const class Person *);

class Person
{
public:
    string mStringID;
    const string mAccount;
    long mSumIncome;
    long mSumExpense;
public:
    Person(string& stringID, const string& account) : mStringID(stringID), mAccount(account) { mSumIncome = 0; mSumExpense = 0; }
    inline void Income(int amount) { mSumIncome += amount; }
    inline void Expense(int amount) { mSumExpense += amount; }
    static inline bool LesserID(const string& a, const Person* b) { return a < b->mStringID; }
    static inline bool GreaterID(const string& a, const Person* b) { return a > b->mStringID; }
    static inline bool EqualID(const string& a, const Person* b) { return a == b->mStringID; }
    static inline bool LesserAcc(const string& a, const Person* b) { return a < b->mAccount; }
    static inline bool GreaterAcc(const string& a, const Person* b) { return a > b->mAccount; }
    static inline bool EqualAcc(const string& a, const Person* b) { return a == b->mAccount; }
};


class CTaxRegister
{
private:
    vector<Person*> people;
    vector<Person*> peopleByAcc;
    const string separator;
private:
    long Find(string id, bool& exactly, comp less, comp great, comp equal, const vector<Person*>* cont) const;
public:
    CTaxRegister(void) : separator("-|-") {};
    ~CTaxRegister(void);
    bool Birth(const string& name, const string& addr, const string& account);
    bool Death(const string& name, const string& addr);
    bool Income(const string& account, int amount);
    bool Income(const string& name, const string& addr, int amount);
    bool Expense(const string& acount, int amount);
    bool Expense(const string& name, const string& addr, int amount);
    bool Audit(const string& name, const string& addr, string& account, int & sumIncome, int & sumExpense) const;
};

long CTaxRegister::Find(string id, bool& exactly, comp less, comp great, comp equal, const vector<Person*>* cont) const
{
    long start = 0;
    long middle = 0;
    long end = cont->size() - 1;

    if(end == -1)
    {
        exactly = false;
        return end;
    }

    while (start <= end)
    {
        middle = (start + end) / 2;
        //cout << middle << endl;
        if (less(id, cont->at(middle)))
        {
           end = middle - 1;
        }
        else if (great(id, cont->at(middle)))
        {
            start = middle + 1;
        }
        else
        {
            break;
        }
    }

    if (equal(id, cont->at(middle)))
        exactly = true;
    else
    {
        exactly = false;
        if(great(id, cont->at(middle)))
            middle++;
    }

    return middle;
}

bool CTaxRegister::Birth(const string& name, const string& addr, const string& account)
{
    if(name.empty() || addr.empty())
    {
        retFalse();
        return false;
    }

    string id = name + separator + addr;
    bool exactly = false;
    long pos = Find(id, exactly, Person::LesserID, Person::GreaterID, Person::EqualID, &people);
    if(exactly)
    {
        retFalse();
        return false;
    }

    bool exactly2 = false;
    long pos2 = Find(account, exactly2, Person::LesserAcc, Person::GreaterAcc, Person::EqualAcc, &peopleByAcc);
    if(exactly2)
    {
        retFalse();
        return false;
    }

    //cout << pos << " - " << pos2 << endl;
    Person* kiddie = new Person(id, account);
    if(pos == (long)people.size() || pos == -1)
    {
        people.push_back(kiddie);
    }
    else
    {
        people.insert(people.begin() + pos, kiddie);
    }
    
    if(pos2 == (long)peopleByAcc.size() || pos2 == -1)
    {
        peopleByAcc.push_back(kiddie);
    }
    else
    {
        peopleByAcc.insert(peopleByAcc.begin() + pos2, kiddie);
    }

    return true;
}

bool CTaxRegister::Income(const string& account, int amount)
{
    if(account.empty())
    {
        retFalse();
        return false;
    }

    bool exactly = false;
    long pos = Find(account, exactly, Person::LesserAcc, Person::GreaterAcc, Person::EqualAcc, &peopleByAcc);
    if(!exactly)
    {
        retFalse();
        return false;
    }


    Person* ritchie = peopleByAcc.at(pos);
    ritchie->Income(amount);
    return true;
}

bool CTaxRegister::Income(const string& name, const string& addr, int amount)
{
    if(name.empty() || addr.empty())
    {
        retFalse();
        return false;
    }

    string id = name + separator + addr;
    bool exactly = false;
    long pos = Find(id, exactly, Person::LesserID, Person::GreaterID, Person::EqualID, &people);
    if(!exactly)
    {
        retFalse();
        return false;
    }

    Person* ritchie = people.at(pos);
    ritchie->Income(amount);
     return true;
}

bool CTaxRegister::Expense(const string& account, int amount)
{
    if(account.empty())
    {
        retFalse();
        return false;
    }

    bool exactly = false;
    long pos = Find(account, exactly, Person::LesserAcc, Person::GreaterAcc, Person::EqualAcc, &peopleByAcc);
    if(!exactly)
    {
        retFalse();
        return false;
    }

    Person* poorman = peopleByAcc.at(pos);
    poorman->Expense(amount);
    return true;
}

bool CTaxRegister::Expense(const string& name, const string& addr, int amount)
{
    if(name.empty() || addr.empty())
    {
        retFalse();
        return false;
    }

    string id = name + separator + addr;
    bool exactly = false;
    long pos = Find(id, exactly, Person::LesserID, Person::GreaterID, Person::EqualID, &people);
    if(!exactly)
    {
        retFalse();
        return false;
    }

    Person* poorman = people.at(pos);
    poorman->Expense(amount);
    return true;
}

bool CTaxRegister::Audit(const string& name, const string& addr, string& account, int & sumIncome, int & sumExpense) const
{
    if(name.empty() || addr.empty())
    {
        retFalse();
        return false;
    }

    string id = name + separator + addr;
    bool exactly = false;
    long pos = Find(id, exactly, Person::LesserID, Person::GreaterID, Person::EqualID, &people);
    if(!exactly)
    {
        retFalse();
        return false;
    }

    const Person* scared = people.at(pos);
    account = scared->mAccount;
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

    string id = name + separator + addr;
    bool exactly = false;
    long pos = Find(id, exactly, Person::LesserID, Person::GreaterID, Person::EqualID, &people);
    if(!exactly)
    {
        retFalse();
        return false;
    }

    Person* oldie = people.at(pos);
    people.erase(people.begin() + pos);

    pos = Find(oldie->mAccount, exactly, Person::LesserAcc, Person::GreaterAcc, Person::EqualAcc, &peopleByAcc);
    peopleByAcc.erase(peopleByAcc.begin() + pos);
    delete oldie;
    return true;
}

CTaxRegister::~CTaxRegister(void)
{

    for_each(people.begin(), people.end(), [](Person*& corpse){ delete corpse; });
    people.clear();
    peopleByAcc.clear();
}

#ifndef __PROGTEST__
int main(int argc, char* argv[])
{
    bool   status;
string acct;
int    sumIncome, sumExpense;

CTaxRegister b2;
status = b2 . Birth ( "John Smith", "Oak Road 23", "123/456/789" );
assert(status, true);
status = b2 . Birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" );
assert(status, true);
status = b2 . Income ( "634oddT", 4000 );
assert(status, false);
status = b2 . Expense ( "John Smith", "Main Street 18", 500 );
assert(status, false);
status = b2 . Audit ( "John Nowak", "Second Street 23", acct, sumIncome, sumExpense );
assert(status, false);
status = b2 . Death ( "Peter Nowak", "5-th Avenue" );
assert(status, false);
status = b2 . Birth ( "Jane Hacker", "Main Street 17", "4et689A" );
assert(status, false);
status = b2 . Birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" );
assert(status, false);
status = b2 . Death ( "Jane Hacker", "Main Street 17" );
assert(status, true);
status = b2 . Birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" );
assert(status, true);
status = b2 . Audit ( "Joe Hacker", "Elm Street 23", acct, sumIncome, sumExpense );
assert(status, true);//, acct = "Xuj5#94", sumIncome=0, sumExpense=0
status = b2 . Birth ( "Joe Hacker", "Elm Street 23", "AAj5#94" );
assert(status, false);


    return 0;
}
#endif //__PROGTEST__                  

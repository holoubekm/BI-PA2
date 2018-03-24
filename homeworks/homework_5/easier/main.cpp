#ifndef __PROGTEST__ // !! DO NOT remove
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>

using namespace std;


//=================================================================================================

class CTimeStamp
{
public:

    CTimeStamp(int year, int month, int day, int hour, int minute, int sec)
    : mYear(year), mMonth(month), mDay(day), mHour(hour), mMin(minute), mSec(sec)
    {
        m_Value = sec + 60 * (minute + 60 * (hour + 24 * (day + 30 * (month + 365 * (long long) year))));
    }

    int Compare(const CTimeStamp & x) const
    {
        if (m_Value < x.m_Value) return -1;
        if (m_Value == x.m_Value) return 0;
        return 1;
    }

    friend ostream & operator <<(ostream & os, const CTimeStamp & x)
    {
        os << x.mYear << '-' << x.mMonth << '-' << x.mDay << ' ' << x.mHour << ':' << x.mMin << ':' << x.mSec; 
        return os;
    }

    long long m_Value;
    int mYear;
    int mMonth;
    int mDay;
    int mHour;
    int mMin;
    int mSec;
};
//=================================================================================================

class CMailBody
{
public:

    CMailBody(int size, const char* data) : m_Size(size)
    {
        m_Data = new char[size + 1];
        strcpy(m_Data, data);
    }

    CMailBody(const CMailBody& ref)
    {
        m_Size = ref.m_Size;
        m_Data = new char[m_Size + 1];
        strcpy(m_Data, ref.m_Data);
    }

    ~CMailBody()
    {
        delete[] m_Data;
    }
  
    friend ostream & operator <<(ostream & os, const CMailBody & x)
    {
        return os << "mail body: " << x . m_Size << " B";
    }
private:
    int m_Size;
    char * m_Data;
};
//=================================================================================================

class CAttach
{
public:

    CAttach(int x)
    : m_X(x), m_RefCnt(1)
    {}

    void AddRef(void) const
    {
        m_RefCnt++;
    }

    void Release(void) const
    {
        if (!--m_RefCnt) delete this;
    }
private:
    int m_X;
    mutable int m_RefCnt;
    CAttach(const CAttach & x);
    CAttach & operator =(const CAttach & x);

    ~CAttach(void)
    {
    }

    friend ostream & operator <<(ostream & os,
            const CAttach & x)
    {
        return os << "+ attachment: " << x . m_X << " B";
    }
};
//=================================================================================================
#endif /* __PROGTEST__, DO NOT remove */

class CMail
{
public:
    inline CMail(const CTimeStamp& time, const string& from, const CMailBody& body, const CAttach* attach) noexcept(true)
    : mTime(time), mFrom(from), mBody(body), mAttach(attach)
    {
        if (mAttach != NULL) 
        {
            mAttach->AddRef();
        }
    }

    inline CMail(const CMail& ref) noexcept(true)
    : mTime(ref.mTime), mFrom(ref.mFrom), mBody(ref.mBody), mAttach(ref.mAttach)
    {
    	if (mAttach != NULL) 
        {
            mAttach->AddRef();
        }
    }

     inline CMail(const CMail&& ref) noexcept(true)
    : mTime(ref.mTime), mFrom(ref.mFrom), mBody(ref.mBody), mAttach(ref.mAttach)
    {
    	cout << "special" << endl;
    	if (mAttach != NULL) 
        {
            mAttach->AddRef();
        }
    }


    inline ~CMail()
    {
        //cout << "removing mail" << endl;
        if (mAttach != NULL) 
        {
            mAttach->Release();
        }
    }

    const string & From(void) const
    {
        return mFrom;
    }

    const CMailBody & Body(void) const
    {
        return mBody;
    }

    const CTimeStamp & TimeStamp(void) const
    {
        return mTime;
    }

    struct Comparator
    {
        bool operator() (const CMail& a, const CMail& b) const
        {
            return a.mTime.Compare(b.mTime) < 0;
        }
    };

    struct TimeComparator
    {

        bool operator() (const CMail& a, const CTimeStamp& b)
        {
            return a.TimeStamp().Compare(b) < 0;
        }

        bool operator() (const CTimeStamp& b, const CMail& a)
        {
            return b.Compare(a.TimeStamp()) < 0;
        }
    };

    const CAttach* Attachment(void) const
    {
        return mAttach;
    }

    friend ostream & operator <<(ostream & os, const CMail & x)
    {
        os << x.TimeStamp() << ' ' << x.From() << ' ' << x.Body();
        if (x.Attachment()) os << ' ' << *x.Attachment();
        return os;
    }
private:
    CTimeStamp mTime;
    string mFrom;
    CMailBody mBody;
    const CAttach* mAttach;
};
//=================================================================================================

class CMailBox
{
public:

    CMailBox(void) : mInbox("inbox")
    {
        mFolders[mInbox];
    }

    ~CMailBox()
    {

    }

    inline bool Delivery(const CMail& mail) noexcept(true)
    {
        mFolders[mInbox].insert(mail);
        return true;
    }

    inline bool NewFolder(const string& folder) noexcept(true)
    {
        if (mFolders.count(folder) != 0) return false;

        mFolders[folder];
        return true;
    }

    inline bool MoveMail(const string& from, const string& to) noexcept(true)
    {
        if (mFolders.count(from) == 0 || mFolders.count(to) == 0)
            return false;

        set<CMail, CMail::Comparator>& moved = mFolders[from];
        set<CMail, CMail::Comparator>& moveTo = mFolders[to];
        set<CMail, CMail::Comparator>::iterator begin = moved.begin();
        set<CMail, CMail::Comparator>::iterator end = moved.end();
        moveTo.insert(moved.begin(), moved.end());
        moved.clear();
        return true;
    }

    list<CMail> ListMail(const string& folderName, const CTimeStamp& from, const CTimeStamp& to) const
    {
        if (mFolders.count(folderName) == 0) return list<CMail>();

        list<CMail> data;
        const set<CMail, CMail::Comparator>& folder = mFolders.at(folderName);
        set<CMail, CMail::Comparator>::const_iterator start = lower_bound(folder.begin(), folder.end(), from, CMail::TimeComparator());
        set<CMail, CMail::Comparator>::const_iterator end = upper_bound(folder.begin(), folder.end(), to, CMail::TimeComparator());

        data.insert(data.begin(), start, end);
        return data;
    }

    set<string> ListAddr(const CTimeStamp & from, const CTimeStamp & to) const
    {
        set<string> values;
        for (const pair<string, set<CMail, CMail::Comparator> >& x : mFolders) 
        {
            const set<CMail, CMail::Comparator>& folder = x.second;
            if (folder.size() == 0) continue;

            set<CMail, CMail::Comparator>::const_iterator start = lower_bound(folder.begin(), folder.end(), from, CMail::TimeComparator());
            set<CMail, CMail::Comparator>::const_iterator end = upper_bound(folder.begin(), folder.end(), to, CMail::TimeComparator());
            while (start != end) 
            {
                values.insert(start->From());
                ++start;
            }
        }
        return values;
    }
private:
    string mInbox;
    map<string, set<CMail, CMail::Comparator> > mFolders;
};
//=================================================================================================

#ifndef __PROGTEST__ /* DO NOT remove */

void showMail(const list<CMail> & l)
{
    cout << "Found: " << l.size() << endl;
    for (list<CMail>::const_iterator it = l . begin(); it != l . end(); ++it)
        cout << *it << endl;
}

void showUsers(const set<string> & s)
{
    for (set<string>::const_iterator it = s . begin(); it != s . end(); ++it)
        cout << *it << endl;
}

int main(void)
{

    bool st = true;
    if(st == true) st = true;
    list<CMail> mailList;
    set<string> users;
    CAttach * att;

CMailBox mm;
    st = mm.Delivery(CMail(CTimeStamp(2014, 3, 31, 15, 24, 13), "user1@fit.cvut.cz", CMailBody(14, "mail content 1"), NULL));
    att = new CAttach(500);
    st = mm.Delivery(CMail(CTimeStamp(2014, 3, 31, 15, 26, 23), "user2@fit.cvut.cz", CMailBody(22, "some different content"), att));
    att->Release();
    st = mm.Delivery(CMail(CTimeStamp(2014, 3, 31, 11, 23, 43), "boss1@fit.cvut.cz", CMailBody(14, "urgent message"), NULL));

return true;
    CMailBox m1;
    st = m1 . NewFolder("work");
    // st = true
    st = m1 . NewFolder("spam");
    // st = true
    st = m1 . NewFolder("spam");
    // st = false
    st = m1 . Delivery(CMail(CTimeStamp(2014, 3, 31, 15, 24, 13), "user1@fit.cvut.cz", CMailBody(14, "mail content 1"), NULL));
    // st = true
    att = new CAttach(500);
    st = m1 . Delivery(CMail(CTimeStamp(2014, 3, 31, 15, 26, 23), "user2@fit.cvut.cz", CMailBody(22, "some different content"), att));
    // st = true
    att -> Release();
    st = m1 . Delivery(CMail(CTimeStamp(2014, 3, 31, 11, 23, 43), "boss1@fit.cvut.cz", CMailBody(14, "urgent message"), NULL));
    // st = true
    att = new CAttach(468);
    st = m1 . Delivery(CMail(CTimeStamp(2014, 3, 31, 18, 52, 27), "user1@fit.cvut.cz", CMailBody(14, "mail content 2"), att));
    // st = true
    att -> Release();
    st = m1 . Delivery(CMail(CTimeStamp(2014, 3, 31, 16, 12, 48), "boss1@fit.cvut.cz", CMailBody(24, "even more urgent message"), NULL));
    // st = true
    showMail(m1 . ListMail("inbox",CTimeStamp(2000, 1, 1, 0, 0, 0), CTimeStamp(2050, 12, 31, 23, 59, 59)));
    /*
    ---8<---8<---8<---8<---8<---8<---
    2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B
    2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B
    2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B
    2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B
    2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B
    ---8<---8<---8<---8<---8<---8<---
     */
    showMail(m1 . ListMail("work",CTimeStamp(2000, 1, 1, 0, 0, 0),CTimeStamp(2050, 12, 31, 23, 59, 59)));
    /*
    ---8<---8<---8<---8<---8<---8<---
    ---8<---8<---8<---8<---8<---8<---
     */
    st = m1 . MoveMail("inbox", "work");
    // st = true
    showMail(m1 . ListMail("inbox",CTimeStamp(2000, 1, 1, 0, 0, 0),CTimeStamp(2050, 12, 31, 23, 59, 59)));
    /*
    ---8<---8<---8<---8<---8<---8<---
    ---8<---8<---8<---8<---8<---8<---
     */
    showMail(m1 . ListMail("work",CTimeStamp(2000, 1, 1, 0, 0, 0),CTimeStamp(2050, 12, 31, 23, 59, 59)));
    /*
    ---8<---8<---8<---8<---8<---8<---
    2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B
    2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B
    2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B
    2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B
    2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B
    ---8<---8<---8<---8<---8<---8<---
     */
    st = m1 . Delivery(CMail(CTimeStamp(2014, 3, 31, 19, 24, 13), "user2@fit.cvut.cz", CMailBody(14, "mail content 4"), NULL));
    // st = true
    att = new CAttach(234);
    st = m1 . Delivery(CMail(CTimeStamp(2014, 3, 31, 13, 26, 23), "user3@fit.cvut.cz", CMailBody(9, "complains"), att));
    // st = true
    att -> Release();
    showMail(m1 . ListMail("inbox",CTimeStamp(2000, 1, 1, 0, 0, 0),CTimeStamp(2050, 12, 31, 23, 59, 59)));
    /*
    ---8<---8<---8<---8<---8<---8<---
    2014-03-31 13:26:23 user3@fit.cvut.cz mail body: 9 B + attachment: 234 B
    2014-03-31 19:24:13 user2@fit.cvut.cz mail body: 14 B
    ---8<---8<---8<---8<---8<---8<---
     */
    showMail(m1 . ListMail("work",CTimeStamp(2000, 1, 1, 0, 0, 0),CTimeStamp(2050, 12, 31, 23, 59, 59)));
    /*
    ---8<---8<---8<---8<---8<---8<---
    2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B
    2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B
    2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B
    2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B
    2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B
    ---8<---8<---8<---8<---8<---8<---
     */
    st = m1 . MoveMail("inbox", "work");
    // st = true
    showMail(m1 . ListMail("inbox",CTimeStamp(2000, 1, 1, 0, 0, 0),CTimeStamp(2050, 12, 31, 23, 59, 59)));
    /*
    ---8<---8<---8<---8<---8<---8<---
    ---8<---8<---8<---8<---8<---8<---
     */
    showMail(m1 . ListMail("work",CTimeStamp(2000, 1, 1, 0, 0, 0),CTimeStamp(2050, 12, 31, 23, 59, 59)));
    /*
    ---8<---8<---8<---8<---8<---8<---
    2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B
    2014-03-31 13:26:23 user3@fit.cvut.cz mail body: 9 B + attachment: 234 B
    2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B
    2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B
    2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B
    2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B
    2014-03-31 19:24:13 user2@fit.cvut.cz mail body: 14 B
    ---8<---8<---8<---8<---8<---8<---
     */
    return 0;
}
#endif /* __PROGTEST__, DO NOT remove */

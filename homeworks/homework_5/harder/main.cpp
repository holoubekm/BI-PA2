#ifndef __PROGTEST__
#include <cstring>  
#include <iostream> 
#include <iomanip>  
#include <set>
#include <list>
#include <vector>
#include <map>   
#include <algorithm>
#include <functional>
using namespace std; 
#endif /* __PROGTEST__ */


#ifndef __PROGTEST__
	#define debug() cout << "Return false at: " << __LINE__ << endl;
#else
	#define debug()
#endif


/*
class CStrComparator
 {
   public:
                   CStrComparator ( bool caseSensitive )
                    : m_CaseSensitive ( caseSensitive ) { }
    bool           operator () ( const string & a, const string & b ) const
     {
       return m_CaseSensitive ? strcasecmp ( a . c_str (), b . c_str () ) < 0 : a < b ;
     }
    bool           m_CaseSensitive;
 };

bool upperCaseCompare ( const char & a, const char & b )
 {
   return toupper ( a ) < toupper ( b );
 }
*/

template <typename T, typename C = std::less<typename T::value_type> >
class CIndex
{
public:
	CIndex(const T& seq, const C& cmp = C());
	~CIndex();
	set<int> Search(const T& query) const;
private:
	int mLen;
	T mData;
	const C& mCmp;
};

template <typename T, typename C>
CIndex<T, C>::CIndex(const T& seq, const C& cmp) : mLen(seq.size()), mData(seq), mCmp(cmp)
{
}

template <typename T, typename C>
CIndex<T, C>::~CIndex()
{

}

template <typename T, typename C>
set<int> CIndex<T, C>::Search(const T& qur) const
{
	int qSize = qur.size();
	
	set<int> results;
	if(qSize == 0)
	{
		vector<int> numbers;
		for (int x = 0; x < mLen; x++)
		    numbers.push_back(x);

		results.insert(numbers.begin(), numbers.end());
		return results;
	}

	typename T::const_iterator curData = mData.begin();
	typename T::const_iterator tempData;
	typename T::const_iterator endData = mData.end();
	typename T::const_iterator curQur = qur.begin();
	typename T::const_iterator endQur = qur.end();

	for(int x = 0; curData != endData; ++curData, x++)
	{
		if(!mCmp(*curData, *qur.begin()) && !mCmp(*qur.begin(), *curData) && (mLen - x >= qSize))
		{
			
			bool ok = true;
			int y = 1;
			cout << "start at: " << x << endl;
			for(curQur = qur.begin(), curQur++, tempData = curData, tempData++; curQur != endQur; ++curQur, ++tempData, y++)
			{
				if(x + y >= mLen)
				{
					debug();
					ok = false;
					break;
				}

				if(mCmp(*tempData, *curQur) || mCmp(*curQur, *tempData))
				{

					debug();
					ok = false;
					break;
				}
			}
			/*
			if(y != qSize - 1)
			{
				debug();	
				ok = false;
			}*/

			if(ok)
			{
				results.insert(x);
			}
		}
	}

	return results;
}
/*
int main()
{
	CIndex<string> a("hello");
	a.Search("a");
	return 0;
}
*/
/*
bool cmp(const char& a, const char& b)
{
	return a < b;
}



int main(int argc, char* argv[])
{
	set<int> ret;
	
CIndex <string, bool(*)(const char &, const char & )> t5 ( "automatIc authentication automotive auTOmation raut", upperCaseCompare );
set<int> r17 = t5 . Search ( "auto" );
 // 0 25 36
set<int> r18 = t5 . Search ( "aut" );
 // 0 10 25 36 48
set<int> r19 = t5 . Search ( "tic" );
 // 6 16
set<int> r20 = t5 . Search ( "trunk" );
 //
set<int> r21 = t5 . Search ( "a" );
 // 0 5 10 19 25 36 41 48
set<int> r22 = t5 . Search ( "" );
 // 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 
 // 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50

CIndex <list<string>, CStrComparator> t6 ( list<string>{ "Hello", "world", "test", "this", "foo", "TEsT", "this", "done" }, CStrComparator ( false ) );
set<int> r23 = t6 . Search ( list<string>{"test", "this", "foo"} );
 // 2
set<int> r24 = t6 . Search ( list<string>{"test", "this"} );
 // 2

CIndex <list<string>, CStrComparator> t7 ( list<string>{ "Hello", "world", "test", "this", "foo", "TEsT", "this", "done" }, CStrComparator ( true ) );
set<int> r25 = t7 . Search ( list<string>{"test", "this", "foo"} );
 // 2
set<int> r26 = t7 . Search ( list<string>{"test", "this"} );
 // 2 5

	ret = r26;
	cout << "Size: " << ret.size() << endl;
	for_each(ret.begin(), ret.end(), [](const int& val)
	{
		cout << ", " << val;
	});
	return 0;
}*/


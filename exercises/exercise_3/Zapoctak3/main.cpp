#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
using namespace std;
#endif /* __PROGTEST__ */
 
class CCityException
 {
   public:
	friend ostream& operator<<(ostream &os, const CCityException& ref){
		os << "exception thrown" << endl;
		return os;
	}
 };
 
struct TCity
 {
                   TCity   ( const string & name, int stay )
                   : m_Next ( NULL ), m_Name ( name ), m_Stay ( stay ) { }
   TCity         * m_Next;
   string          m_Name;
   int             m_Stay;
 };
 

class CRes
{
public:
	CRes(){}
	CRes(TCity* begin, int count, int stay) : mBegin(begin), mCount(count), mStay(stay) {
	}

	friend ostream& operator<<(ostream& os, const CRes& ref){
		TCity* cur = ref.mBegin;
		os << "Total: " << ref.mStay << endl;
		for(int x = 0; x < ref.mCount; x++){
			os << cur->m_Name  << " - " << cur->m_Stay << endl;
			cur = cur->m_Next;
		}
		return os << endl;
	}

	int Stay() const{
		return mStay;
	}

private:
	TCity* mBegin;
	int mCount;
	int mStay;
};

class CTour
{
	public:
	CTour() : m_Cities(NULL), mLast(NULL), mCnt(0) {
	}

	~CTour(){
		Release();
	}

	void Release(){
		TCity* toDel, *cur = m_Cities;
		for(int x = 0; x < mCnt; x++){
			toDel = cur;
			cur = cur->m_Next;
			delete toDel;
		}

		m_Cities = NULL;
		mLast = NULL;
		mCnt = 0;
	}

	CTour& operator=(const CTour& ref){
		if(this == &ref) return *this;
		if(m_Cities != NULL) Release(); m_Cities = NULL;
		mCnt = 0;
		TCity* cur = ref.m_Cities;
		for(int x = 0; x < ref.mCnt; x++){
			TCity* newCity = new TCity(*cur); newCity->m_Next = NULL;
			Insert(newCity);
			cur = cur->m_Next;
		}

		return *this;
	}

	CRes Trip(const string& from, const string& to) const{
		TCity* cur = m_Cities;
		TCity* From = NULL;
		int stay = 0, cnt = 0;

		for(int x = 0; x < mCnt; x++){
			if(cur->m_Name == from){
				From = cur;
				stay += cur->m_Stay;
				cnt++;
				break;
			}
			cur = cur->m_Next;
		}

		if(From == NULL){
			throw CCityException();
		}

		for(int x = 0; x < mCnt; x++){
			if(cur->m_Name == to){
				break;
			}
			cnt++;
			cur = cur->m_Next;
			stay += cur->m_Stay;
		}

		return CRes(From, cnt, stay);
	}

	void Add(istream& in){
		string line;
		while(getline(in, line)){
			if(line.size() == 0) continue;
			istringstream lines(line);
			string city, days;
			getline(lines, city, ';');
			getline(lines, days);
			istringstream stays(days);
			int stay;
			stays >> stay;
			lines >> city >> days;
			Insert(new TCity(city, stay));
		}
	}

	void Insert(TCity* city){
		mCnt++;
		if(m_Cities == NULL){
			m_Cities = city;
			mLast = city;
			mLast->m_Next =	m_Cities;
			return;
		}

		mLast->m_Next = city;
		mLast = city;
		mLast->m_Next = m_Cities;
	}

   private:
    // todo
    TCity* m_Cities;
    TCity* mLast;
	int mCnt;
	friend class CTester;
};

int main(int argc, char* argv[])
{
	
	istringstream is;
	int           stay;
	CTour a;
	is . clear ();
	is . str ( "London;1\nMumbai;3\nKolkata;2\nHong Kong;1\nYokohama;2\nSan Francisco;3\nNew York City;2\n" );
	a . Add ( is );
	stay = a . Trip ( "Mumbai", "San Francisco" ) . Stay (); // stay = 11
	cout << a . Trip ( "Mumbai", "San Francisco" );
	/*
	--8<----8<----8<----8<----8<----8<----8<--
	Mumbai
	-> Kolkata
	-> Hong Kong
	-> Yokohama
	-> San Francisco
	--8<----8<----8<----8<----8<----8<----8<--
	*/
 
	stay = a . Trip ( "Yokohama", "Mumbai" ) . Stay (); // stay = 11
	cout << a . Trip ( "Yokohama", "Mumbai" );
	/*
	--8<----8<----8<----8<----8<----8<----8<--
	Yokohama
	-> San Francisco
	-> New York City
	-> London
	-> Mumbai
	--8<----8<----8<----8<----8<----8<----8<--
	*/
 
	stay = a . Trip ( "Kolkata", "Kolkata" ) . Stay (); // stay = 2
	cout << a . Trip ( "Kolkata", "Kolkata" );
	/*
	--8<----8<----8<----8<----8<----8<----8<--
	Kolkata
	--8<----8<----8<----8<----8<----8<----8<--
	*/
 
	is . clear ();
	is . str ( "Dublin;1\nLiverpool;1" );
	a . Add ( is );
	stay = a . Trip ( "San Francisco", "Mumbai" ) . Stay (); // stay = 11
	cout << a . Trip ( "San Francisco", "Mumbai" );
	/*
	--8<----8<----8<----8<----8<----8<----8<--
	San Francisco
	-> New York City
	-> Dublin
	-> Liverpool
	-> London
	-> Mumbai
	--8<----8<----8<----8<----8<----8<----8<--
	*/

	try { cout << a . Trip ( "Shanghai", "London" ); }
	catch ( const CCityException & e ) { cout << e; }

	cout << "beeeeeeeeeeeeeeeeeeeee" << endl << endl << endl;
	CTour b;
	is . clear ();
	is . str ( "London;1\nMumbai;3\nKolkata;2\n\n\nHong Kong;1\nYokohama;2\nSan Francisco;3\n" );
	b . Add ( is );
	stay = b . Trip ( "Mumbai", "San Francisco" ) . Stay (); // stay = 11
	cout << b . Trip ( "Mumbai", "San Francisco" );
	/*
	--8<----8<----8<----8<----8<----8<----8<--
	Mumbai
	-> Kolkata
	-> Hong Kong
	-> Yokohama
	-> San Francisco
	--8<----8<----8<----8<----8<----8<----8<--
	*/
 
	is . clear ();
	is . str ( "New York City;2\nNew York City;2\nLiverpool;2" );
	b . Add ( is );
	stay = b . Trip ( "Yokohama", "London" ) . Stay (); // stay = 12
	cout << b . Trip ( "Yokohama", "London" );
	/*
	--8<----8<----8<----8<----8<----8<----8<--
	Yokohama
	-> San Francisco
	-> New York City
	-> New York City
	-> Liverpool
	-> London
	--8<----8<----8<----8<----8<----8<----8<--
	*/
 
	stay = b . Trip ( "Kolkata", "Liverpool" ) . Stay (); // stay = 14
	cout << b . Trip ( "Kolkata", "Liverpool" );
	/*
	--8<----8<----8<----8<----8<----8<----8<--
	Kolkata
	-> Hong Kong
	-> Yokohama
	-> San Francisco
	-> New York City
	-> New York City
	-> Liverpool
	--8<----8<----8<----8<----8<----8<----8<--
	*/
 
	CTour c;
	is . clear ();
	is . str ( "London;1\nMumbai;3\nKolkata;2\nHong Kong;1\n" );
	c . Add ( is );
	CTour d;
	is . clear ();
	is . str ( "San Francisco;3\nNew York City;2\nLondon;1\n" );
	d . Add ( is );
	d = c;
	is . clear ();
	is . str ( "Yokohama;2\nSan Francisco;3\nNew York City;2\nLiverpool;1\n" );
	c . Add ( is );
	is . clear ();
	is . str ( "Shanghai;1\nYokohama;1\nSan Francisco;1\nNew York City;4\nDublin;2\nLiverpool;1\n" );
	d . Add ( is );
	stay = c . Trip ( "Kolkata", "London" ) . Stay (); // stay = 12
	cout << c . Trip ( "Kolkata", "London" );
	/*
	--8<----8<----8<----8<----8<----8<----8<--
	Kolkata
	-> Hong Kong
	-> Yokohama
	-> San Francisco
	-> New York City
	-> Liverpool
	-> London
	--8<----8<----8<----8<----8<----8<----8<--
	*/
 
	stay = d . Trip ( "Kolkata", "London" ) . Stay (); // stay = 14
	cout << d . Trip ( "Kolkata", "London" );
	/*
	--8<----8<----8<----8<----8<----8<----8<--
	Kolkata
	-> Hong Kong
	-> Shanghai
	-> Yokohama
	-> San Francisco
	-> New York City
	-> Dublin
	-> Liverpool
	-> London
	--8<----8<----8<----8<----8<----8<----8<--
	*/

	system("pause");
	return 0;
}
#ifndef __PROGTEST__
#include "common.h"
using namespace std;
#endif /* __PROGTEST__ */

enum CType
{
	mA,
	mAAAA,
	mMX
};

hash<string> mHasher;

class CRec
{
public:
	CRec(const string& name, CType type) 
	: mName(name), mType(type), mNameHash(mHasher(name)){
	}

	const string Name() const{
		return mName;
	}

	const string Type() const{
		switch(mType)
		{
			case mA:
			return "A";
			case mAAAA:
			return "AAAA";
			case mMX:
			return "MX";
		}
		return "";
	}

	friend ostream& operator<<(ostream& os, const CRec& ref){
		ref.Print(os);
		return os;
	}
	
	virtual bool operator==(const CRec& ref) const{
		if(mType != ref.mType) return false;
		if(mNameHash != ref.mNameHash) return false;
		return GetHash() == ref.GetHash();
	}

	virtual const size_t GetHash() const = 0;
	virtual void Print(ostream& os) const = 0;
public:
	const string mName;
	const CType mType;
	const size_t mNameHash;
};

class CRecA : public CRec
{
public:
	CRecA(const string& name, const CIPv4& ip) : CRec(name, mA), mIP(ip){
		ostringstream input; input << ip;
		mIPHash = mHasher(input.str());
	}

	virtual const size_t GetHash() const{
		return mIPHash;
	}

	virtual void Print(ostream& os) const{
		os << mName << ' ' << Type() << ' ' << mIP << endl;
	}

	const CIPv4 mIP;
	size_t mIPHash;
};

class CRecAAAA : public CRec 
{
public:
	CRecAAAA(const string& name, const CIPv6& ip) : CRec(name, mAAAA), mIP(ip)
	{
		ostringstream input; input << ip;
		mIPHash = mHasher(input.str());
	}

	virtual const size_t GetHash() const{
		return mIPHash;
	}

	virtual void Print(ostream& os) const{
		os << mName << ' ' << Type() << ' ' << mIP << endl;
	}

	const CIPv6 mIP;
	size_t mIPHash;
};

class CRecMX : public CRec
{
public:
	CRecMX(const string& name, const string& server, int priority)
	: CRec(name, mMX), mServer(server), mPriority(priority){
		ostringstream input; input << server;
		mServerHash = mHasher(input.str());
		mServerHash = mServerHash ^ (priority << 1);
	}

	virtual const size_t GetHash() const{
		return mServerHash;
	}

	virtual void Print(ostream& os) const{
		os << mName << ' ' << Type() << ' ' << mPriority << ' ' << mServer << endl;
	}

	const string mServer;
	int mPriority;
	size_t mServerHash;
};

class CSearch
{
public:

	~CSearch(){
		for_each(mEntries.begin(), mEntries.end(), [](shared_ptr<CRec> entry){
			entry.reset();
		});
	}

	int Count() const{
		return mEntries.size();
	}

	void Add(shared_ptr<CRec> entry){
		mEntries.push_back(entry);
	}

	CRec& operator[](int x) const{
		if(x < 0 || x >= (int)mEntries.size()){
			throw InvalidIndexException(x);
		}
		return *mEntries[x];
	}

	friend ostream& operator<<(ostream& os, const CSearch& search)
	{
		for_each(search.mEntries.begin(), search.mEntries.end(), [&](shared_ptr<CRec> entry){
			os << *entry;
		});
		return os;
	}

	vector<shared_ptr<CRec> > mEntries;
};

class CZone
{
public:
	CZone(const string& zone) : mZone(zone){
	}

	~CZone(){
		for_each(mEntries.begin(), mEntries.end(), [](shared_ptr<CRec> entry){
			entry.reset();
		});
	}

	CZone& operator=(const CZone& ref)
	{
		mBuf.clear();
		mBuf = ref.mBuf;
		mEntries.clear();
		mZone = ref.mZone;
		mEntries = vector<shared_ptr<CRec> >(ref.mEntries.begin(), ref.mEntries.end());
		return *this;
	}

	bool Add(const CRec& entry){
		if(find_if(mEntries.begin(), mEntries.end(), [&entry](shared_ptr<CRec> a){
    			return *a == entry;
    		}) != mEntries.end()){
			return false;
		}

		shared_ptr<CRec> newEntry = nullptr;
		switch(entry.mType)
		{
			case mA:
				newEntry = shared_ptr<CRec>(new CRecA((const CRecA&)entry));
				break;
			case mAAAA:
				newEntry = shared_ptr<CRecAAAA>(new CRecAAAA((const CRecAAAA&)entry));
				break;
			case mMX:
				newEntry = shared_ptr<CRecMX>(new CRecMX((const CRecMX&)entry));
				break;
		}
		mEntries.push_back(newEntry);
		mBuf[entry.mName].push_back(newEntry);
		return true;
	}

	bool Del(const CRec& entry){
		vector<shared_ptr<CRec> >::iterator it;
		if((it = find_if(mEntries.begin(), mEntries.end(), [&](shared_ptr<CRec> a){
    			return *a == entry;
    		})) != mEntries.end()){
			
			mEntries.erase(it);

			vector<shared_ptr<CRec> > & rem = mBuf[entry.mName];
			if((it = find_if(rem.begin(), rem.end(), [&](shared_ptr<CRec> a){
    			return *a == entry;
    		})) != rem.end())
    		{
    			rem.erase(it);
			}
			return true;
		}
		return false;
	}

	CSearch Search(const string& name) const{
		CSearch found;
		if(mBuf.count(name))
			found.mEntries = mBuf.at(name);
		return found;
	}

	friend ostream& operator<<(ostream& os, const CZone& zone)
	{
		os << zone.mZone << endl;
		uint x = 0;
		for_each(zone.mEntries.begin(), zone.mEntries.end(), [&](shared_ptr<CRec> entry){
			if(x != zone.mEntries.size() - 1){
				os << " +- " << *entry;
			}else{
				os << " \\- " << *entry;
			}
			x++;
		});
		return os;
	}

	string mZone;
	map<string, vector<shared_ptr<CRec> > > mBuf;
	vector<shared_ptr<CRec> > mEntries;
};

#ifndef __PROGTEST__
int main ( void )
{
	#include "tests.inc"
	return 0;
}
#endif /* __PROGTEST__ */

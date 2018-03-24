#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;
#endif /* __PROGTEST__ */


#ifndef __PROGTEST__
#define mdebug() cout << "Return false at: " << __LINE__ << endl;
#else
#define mdebug()
#endif

#define ALLOC 1

class CString
{
public:
    
    CString() : mLen(0), mStr(NULL), mRef(new int(1)) {}

    CString(const CString& ref) : mLen(ref.mLen), mStr(ref.mStr), mRef(ref.mRef)
    {
        (*mRef)++;
    }

    CString(const char* ref)
    {
        mLen = strlen(ref) + 1;
        mStr = new char[mLen];
        mRef = new int(1);

        strcpy(mStr, ref);
    }

    ~CString()
    {
        (*mRef)--;
        if(*mRef == 0)
        {
            Release();
        }
    }

    void Release()
    {
        delete[] mStr;
        delete mRef;
    }

    CString& operator=(const CString& ref)
    {
        (*mRef)--;
        if(*mRef == 0)
        {
            Release();
        }

        cout << "Assignment" << endl;
        mLen = ref.mLen;
        mStr = ref.mStr;
        mRef = ref.mRef;
        (*mRef)++;

        return *this;
    }

    friend ostream& operator<<(ostream& os, const CString& str)
    {
        os << str.mStr;
        return os;
    } 

    friend bool operator==(const CString& first, const CString& second)
    {
        if(first.mLen != second.mLen) return false;
        return strcmp(first.mStr, second.mStr) == 0;
    }


    friend bool operator>(const CString& first, const CString& second)
    {
        return strcmp(first.mStr, second.mStr) > 0;
    }


    friend bool operator<(const CString& first, const CString& second)
    {
        return strcmp(first.mStr, second.mStr) < 0;
    }

public:
    int     mLen;
    char*   mStr;

private:
    int*    mRef;
};

class CTrans
{

public:
    CTrans() : mAmount(0), mOut(true), mRef(new int(1)) { }

    CTrans(const CTrans& ref) : mPartner(ref.mPartner), mSign(ref.mSign), mAmount(ref.mAmount), mOut(ref.mOut), mRef(ref.mRef) 
    {
        (*mRef)++;
    }

    CTrans(const CString& partner, int amount, const CString& sign, bool out) : mPartner(partner), mSign(sign), mAmount(amount), mOut(out), mRef(new int(1)) { }

    ~CTrans()
    {
        (*mRef)--;
        if(*mRef == 0)
        {
            delete mRef;
        }
    }

    CTrans& operator=(const CTrans& ref)
    {
        (*mRef)--;
        if(*mRef == 0)
        {
            delete mRef;
        }

        mRef = ref.mRef;
        mPartner = ref.mPartner;
        mSign = ref.mSign;
        mAmount = ref.mAmount;
        mOut = ref.mOut;
        (*mRef)++;

        return *this;
    }

    friend ostream& operator<<(ostream& os, const CTrans& trn)
    {
        os << trn.mPartner.mStr << " - " << trn.mAmount << endl;
        return os;
    } 

public:
    CString mPartner;
    CString mSign;
    int     mAmount;
    bool    mOut;

private:
    int* mRef;
};

class CAccount
{
public:

    CAccount() : mBalance(0), mInitial(0), mRef(new int(1)), mNumTrans(0), mMaxTrans(ALLOC)
    {
        mTrans = new CTrans*[mMaxTrans];
    }

    CAccount(const CAccount& ref) : mID(ref.mID), mTrans(ref.mTrans), mBalance(ref.mBalance), mInitial(ref.mInitial), mRef(ref.mRef), mNumTrans(ref.mNumTrans), mMaxTrans(ref.mMaxTrans)
    {
        (*mRef)++;
    }

    CAccount(const CAccount*& ref) : mID(ref->mID), mTrans(ref->mTrans), mBalance(ref->mBalance), mInitial(ref->mInitial), mRef(ref->mRef), mNumTrans(ref->mNumTrans), mMaxTrans(ref->mMaxTrans)
    {
        (*mRef)++;
    }

    CAccount(const CAccount& ref, bool copy) : mID(ref.mID), mBalance(ref.mBalance), mInitial(ref.mInitial), mRef(new int(1)), mNumTrans(ref.mNumTrans), mMaxTrans(ref.mMaxTrans)
    {
        mTrans = new CTrans*[mMaxTrans];
        for(int x = 0; x < mNumTrans; x++)
        {
            mTrans[x] = new CTrans(*ref.mTrans[x]);
        }
    }

    CAccount(const CString& id, int balance) : mID(id), mBalance(balance), mInitial(balance), mRef(new int(1)), mNumTrans(0), mMaxTrans(ALLOC)
    {
        mTrans = new CTrans*[mMaxTrans];
    }

    void Release()
    {
        if(mRef != NULL)
        {
            delete mRef;
            mRef = NULL;
        }

        if(mTrans != NULL)
        {
            for(int x = 0; x < mNumTrans; x++)
            {
                if(mTrans[x] != NULL)
                {
                    delete mTrans[x];
                    mTrans[x] = NULL;
                }
            }
            delete[] mTrans;
        }
    }

    ~CAccount()
    {
        (*mRef)--;
        if(*mRef == 0)
        {
            Release();
        }
    }

    CAccount& operator=(const CAccount& ref)
    {
        (*mRef)--;
        if(*mRef == 0)
        {
            Release();
        }

        mNumTrans = ref.mNumTrans;
        mMaxTrans = ref.mMaxTrans;
        mBalance = ref.mBalance;
        mInitial = ref.mInitial;
        mRef = ref.mRef;
        mID = ref.mID;
        mTrans = ref.mTrans;
        (*mRef)++;

        return *this;
    }

    int Balance()
    {
        return mBalance;
    }

    void Trim()
    {
        if(mTrans != NULL)
        {
            for(int x = 0; x < mNumTrans; x++)
            {
                if(mTrans[x] != NULL)
                {
                    delete mTrans[x];
                    mTrans[x] = NULL;
                }
            }
            delete[] mTrans;
        }

        mTrans = new CTrans*[mMaxTrans = ALLOC];
        mNumTrans = 0;
        mInitial = mBalance;
    }

    void Transaction(const CString& partner, int amount, const CString& sign, bool out)
    {
        if(mNumTrans + 1 > mMaxTrans)
        {
            mMaxTrans *= 2;
            CTrans** buf = new CTrans*[mMaxTrans];
            for(int x = 0; x < mNumTrans; x++)
            {
                buf[x] = mTrans[x];
            }
            delete[] mTrans;
            mTrans = buf;
        }
        mTrans[mNumTrans] = new CTrans(partner, amount, sign, out);

        if(out)
            mBalance = mBalance - amount;
        else
            mBalance = mBalance + amount;
        mNumTrans++;
    }

    friend ostream& operator<<(ostream& os, const CAccount& ob)
    {
        os //<< "---8<---8<---8<---8<---8<---8<---8<---" << endl
        << ob.mID.mStr << ":" << endl << "   " << ob.mInitial << endl;
        for(int x = 0; x < ob.mNumTrans; x++)
        {
            os << (ob.mTrans[x]->mOut ? " - " : " + ") << 
            ob.mTrans[x]->mAmount << ", " << (ob.mTrans[x]->mOut ? "to: " : "from: ") <<
            ob.mTrans[x]->mPartner.mStr << ", sign: " << ob.mTrans[x]->mSign.mStr << endl;
        }

        os << " = " << ob.mBalance << endl;// << "---8<---8<---8<---8<---8<---8<---8<---" << endl;
        return os;
        return os;
    } 

public:
    CString mID;
    CTrans** mTrans;
    int mBalance;
    int mInitial;
    int* mRef;
private:

    int mNumTrans;
    int mMaxTrans;
};



class CBank
{
public:
    CBank()
    {
        mMaxAcc = 32;
        mNumAcc = 0;
        mAcc = new CAccount*[mMaxAcc];
    }

    CBank(const CBank& bank)
    {
        mMaxAcc = bank.mMaxAcc;
        mNumAcc = bank.mNumAcc;
        mAcc = new CAccount*[mMaxAcc];
        for(int x = 0; x < mNumAcc; x++)
        {
            mAcc[x] = new CAccount(*bank.mAcc[x]);
        }
    }

    ~CBank() 
    { 
        if(mAcc != NULL) 
        {
            for(int x = 0; x < mNumAcc; x++)
            {
                delete mAcc[x];
            }
            delete[] mAcc; mAcc = NULL;
        }
    }

    CBank& operator=(const CBank& bank)
    {
        if(mAcc != NULL) 
        {
            int x = 0;
            for(; x < mNumAcc && x < bank.mNumAcc; x++)
            {
                *mAcc[x] = *bank.mAcc[x];
            }

            for(; x < bank.mNumAcc; x++)
            {
                if(x + 1 >= mMaxAcc)
                {
                    mMaxAcc *= 2;
                    CAccount** buf = new CAccount*[mMaxAcc];
                    for(int y = 0; y < mNumAcc; y++)
                    {
                        buf[y] = mAcc[y];
                    }
                    delete[] mAcc;
                    mAcc = buf;
                }
                mAcc[x] = new CAccount(*bank.mAcc[x]);
            }
            mNumAcc = x;
        }
        else
        {
            mAcc = new CAccount*[mMaxAcc];
            for(int x = 0; x < mNumAcc; x++)
            {
                mAcc[x] = new CAccount(*bank.mAcc[x]);
            }
        }


        mMaxAcc = bank.mMaxAcc;
        mNumAcc = bank.mNumAcc;
        
        return *this;
    }

    bool NewAccount(const CString& accID, int initialBalance)
    {
        
        int pos = 0;
        if(FindAccount(accID, pos))
        {
            mdebug();
            return false;
        }

        if(mNumAcc + 1 >= mMaxAcc)
        {
            mMaxAcc *= 2;
            CAccount** buf = new CAccount*[mMaxAcc];
            for(int x = 0; x < mNumAcc; x++)
            {
                buf[x] = mAcc[x];
            }
            delete[] mAcc;
            mAcc = buf;
        }

        mNumAcc++;
        for(int x = mNumAcc; x > pos; x--)
        {
            mAcc[x] = mAcc[x - 1];
        }

        mAcc[pos] = new CAccount(accID, initialBalance);
        return true;

    }

    bool Transaction(const CString& debAccID, const CString& credAccID, int amount, const CString& signature)
    {
        int pos1 = -1, pos2 = -1;
        if(debAccID.mLen == 0 || credAccID.mLen == 0)
            return false;

        if(debAccID == credAccID || !FindAccount(debAccID, pos1) || !FindAccount(credAccID, pos2))
        {
            mdebug();
            return false;
        }

        //mAcc[pos1] = CAccount(mAcc[pos1]);
        //mAcc[pos2] = CAccount(mAcc[pos2]);

        if((*mAcc[pos1]->mRef) > 1) 
        {
            CAccount* buf = new CAccount(*mAcc[pos1], true);
            delete mAcc[pos1];
            mAcc[pos1] = buf;
        }

        if((*mAcc[pos2]->mRef) > 1) 
        {
            CAccount* buf = new CAccount(*mAcc[pos2], true);
            delete mAcc[pos2];
            mAcc[pos2] = buf;
        }
        mAcc[pos1]->Transaction(mAcc[pos2]->mID, amount, signature, true);
        mAcc[pos2]->Transaction(mAcc[pos1]->mID, amount, signature, false);

        return true;
    }

    bool TrimAccount(const CString& accID)
    {
        int pos = -1;
        if(!FindAccount(accID, pos))
        {
            mdebug();
            return false;
        }
        mAcc[pos]->Trim();

        return true;
    }

    CAccount& Account(const CString& accID)
    {
        for(int x = 0; x < mNumAcc; x++)
        {
            if(accID == mAcc[x]->mID)
            {
                return *mAcc[x];
            }
        }

        throw "exception";
        return *mAcc[0];
    }

private:
    bool FindAccount(const CString& ref, int& pos)
    {
        bool state = true;
        long start = 0;
        long middle = 0;
        long end = mNumAcc - 1;

        if(end == -1)
        {
            return false;
        }

        while (start <= end)
        {
            middle = (start + end) / 2;
            //cout << middle << endl;
            if (ref < mAcc[middle]->mID)
            {
               end = middle - 1;
            }
            else if (ref > mAcc[middle]->mID)
            {
                start = middle + 1;
            }
            else
            {
                break;
            }
        }

        if (ref == mAcc[middle]->mID)
            state = true;
        else
        {
            state = false;
            if(ref > mAcc[middle]->mID)
                middle++;
        }

        pos = middle;
        return state;
    }

public:
    CAccount** mAcc;
    int mMaxAcc;
    int mNumAcc;
};


#ifndef __PROGTEST__
int main(int argc, char* argv[])
{
    CBank a;
    CBank b;

    a.NewAccount("martin", 400);
    a.NewAccount("eva", 400);
    a.NewAccount("test2", 400);
    a.NewAccount("test3", 400);
    a.NewAccount("test4", 400);
    a.NewAccount("test5", 400);
    a.NewAccount("test6", 400);
    a.NewAccount("test7", 400);
    a.NewAccount("test8", 400);

    b.NewAccount("asdf1", 400);
    b.NewAccount("asdf2", 400);
    b.NewAccount("asdf3", 400);
    b.NewAccount("asdf4", 400);
    b.NewAccount("asdf5", 400);
    b.NewAccount("asdf6", 400);

    b = a;
    system("pause");
    return 0;
}
#endif

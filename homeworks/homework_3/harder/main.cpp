#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */


#ifndef __PROGTEST__
#define debug() cout << "False returned at: " << __LINE__ << endl;
#else
#define debug()
#endif


class CBigInt
{
public:
	CBigInt()
	{
		mSize = 1;
		mPos = true;
		mData.push_back(0);
	}

	CBigInt(const CBigInt& ref)
	{
		mPos = ref.mPos;
		mSize = ref.mSize;
		mData = ref.mData;
	}

	CBigInt(const int val)
	{
		int buf = val;
		if(buf == 0)
		{
			mSize = 1;
			mPos = true;
			mData.push_back(0);
			return;
		}

		mSize = 0;
		mPos = true;
		if(buf < 0) 
		{
			mPos = false;
			buf = -buf;
		}

		while(buf > 0)
		{
			mData.push_back(buf - (buf / 10) * 10);
			mSize++;
			buf = buf / 10;
		}
	}

	CBigInt(const char* val)
	{
		mPos = true;
		mData.clear();
		mSize = strlen(val);
		mData.clear();
		int stop = 0;
		if(val[0] == '-') { stop++; }
		for(int x = mSize - 1; x >= stop; x--)
		{
			mData.push_back(val[x] - '0');
		}
		if(val[0] == '-') { mPos = false; mSize--; }
		RemoveLeading();
	}

	~CBigInt() {}

	CBigInt& operator=(const CBigInt& ref)
	{
		mData = ref.mData;
		mSize = ref.mSize;
		mPos = ref.mPos;
		return *this;
	}


	CBigInt& operator=(int ref)
	{
		mData.clear();
		mSize = 0;
		mPos = ref < 0 ? false : true;
		ref = ref < 0 ? -ref : ref;
		while(ref > 0)
		{
			char val = ref - (ref / 10) * 10;
			ref /= 10;
			mData.push_back(val);
			mSize++;
		}
		return *this;
	}

	CBigInt& operator=(const char* val)
	{
		mPos = true;
		mData.clear();
		mSize = strlen(val);
		mData.clear();
		int stop = 0;
		if(val[0] == '-') { stop++; }
		for(int x = mSize - 1; x >= stop; x--)
		{
			mData.push_back(val[x] - '0');
		}
		if(val[0] == '-') { mPos = false; mSize--; }
		RemoveLeading();
		return *this;
	}

	friend ostream& operator<<(ostream& os, const CBigInt& num)
	{
		if(!num.mPos) os << '-';
		for(int x = num.mSize - 1; x >= 0; x--)
		{
			os << (int)num.mData[x];
		}
		return os;
	}

	friend istream& operator>>(istream& is, CBigInt& num)
	{
		num.RemoveLeading();
		
		vector<int> buf;
		is >> noskipws;
		bool sign = true;
		int count = 0;
		bool signloaded = false;
		char dummy;
		while(true)
		{
			is >> dummy;
			if(!is.good()) break;
			if(isspace(dummy))
			{
				if(count > 0)
				{
					break;
				}
				else
				{
					continue;
				}
			}

			if(dummy == '-')
			{
				if(!signloaded)
				{
					sign = false;
					signloaded = true;
				}
				else break;
			}
			else if(isdigit(dummy))
			{
				buf.push_back(dummy - '0');
				count++;
			}
			else
			{
				break;
			}

			
		}
		is.clear();

		if(count > 0)
		{
			num.mSize = (int)buf.size();
			num.mPos = sign;
			num.mData.clear();
			for(int x = buf.size() - 1; x >= 0; x--)
			{
				num.mData.push_back(buf[x]);
			}
		}
		else
		{
			is.setstate(ios::failbit);
		}

		
		return is;
	}
	
	CBigInt operator+(const CBigInt& ref)
	{
		CBigInt buf;
		buf = *this;
		buf += ref;
		return buf;
	}

	CBigInt& operator+=(const CBigInt& ref)
	{
		int sizeThis = mSize;
		int sizeThat = ref.mSize;
		bool same = sizeThis == sizeThat;
		int larger = sizeThis > sizeThat ? sizeThis : sizeThat;
		int carry = 0;
		int value = 0;
		if((mPos && ref.mPos) || (!mPos && !ref.mPos))
		{
			for(int x = 0; x < larger; x++)
			{
				value = carry; carry = 0;
				if(x < sizeThat) value += ref.mData[x];
				if(x < sizeThis) 
					mData[x] += value;
				else
					mData.push_back(value);
				carry = 0;
				if(mData[x] >= 10) { mData[x] -= 10; carry = 1; }
			}
			mSize = larger;
			if(carry == 1) { mData.push_back(carry); mSize++; }
		}
		else
		{
			const CBigInt* lgr = sizeThis > sizeThat ? this : &ref;
			const CBigInt* sml = sizeThis > sizeThat ? &ref : this;
			if(same)
			{
				for(int x = sizeThis - 1; x >= 0; x--)
				{
					if(lgr->mData[x] > sml->mData[x]) break;
					else if(lgr->mData[x] < sml->mData[x])
					{
						const CBigInt* buf = lgr;
						lgr = sml;
						sml = buf;
						break;
					}
				}
			}

			carry = 0;
			for(int x = 0; x < lgr->mSize; x++)
			{
				value = (lgr->mData[x] - carry);
				if(x < sml->mSize) value -= sml->mData[x];
				if(value < 0)
				{
					value += 10;
					carry = 1;
				}

				if(x < mSize)
					mData[x] = value;
				else
					mData.push_back(value);
			}

			mPos = lgr->mPos;
			RemoveLeading();
		}
		return *this;
	}
	
	CBigInt operator*(const CBigInt& ref)
	{
		CBigInt buf;
		buf = *this;
		buf *= ref;
		return buf;
	}


	void RemoveLeading()
	{
		int zeros = 0;
		for(int x = mData.size() - 1; x >= 0; x--)
		{
			if(mData[x] == 0)
				zeros++;
			else
				break;
		}

		mSize = mData.size() - zeros;
		if(mSize == 0) { mSize++; mPos = true; }
		mData.resize(mSize);
	}
	
	CBigInt& operator *=(const CBigInt& ref)
	{
		int alloc = mSize + ref.mSize + 1;
		int in = 0, value = 0, carry = 0;
		int sizeThis = mSize;
		int sizeThat = ref.mSize;
		vector<short> buffer(alloc);

		for(int x = 0; x < alloc; x++)
		{
			value = 0;
			for(int im = 0; im <= x; im++)
			{
				in = x - im;
				if(im < sizeThis && in < sizeThat)
				{
					value += mData[im] * ref.mData[in];
				}
			}
			value += carry;
			buffer[x] = value - (value / 10) * 10;
			carry = value / 10;
			//cout << value << endl;
		}

		mPos = (mPos && ref.mPos) || (!mPos && !ref.mPos) ? true : false;
		mData = buffer;
		RemoveLeading();

		return *this;
	}

public:
	vector<short> mData;
	int mSize;
	bool mPos;
};

#ifndef __PROGTEST__
int main(int argc, char* argv[])
{
CBigInt       a, b;
istringstream is;
a = 10;
a += 20;  // a = 30 
a *= 5;  // a = 150 
cout << a;  // 150
b = a + 3;  // b = 153 
b = a * 7;  // b = 1050 
cout << a;  // 150

a = 10;
a += -20;  // a = -10 
a *= 5;  // a = -50 
cout << a;  // -50
b = a + 73;  // b = 23 
b = a * -7;  // b = 350 
cout << a;  // -50

a = "12345678901234567890";
a += "-99999999999999999999";  // a = -87654321098765432109 
a *= "54321987654321987654";  // a = -4761556948575111126880627366067073182286 
cout << a;  // -4761556948575111126880627366067073182286
a *= 0;  // a = 0 
cout << a;  // 0
a = 10;
b = a + "400";  // b = 410 
b = a * "15";  // b = 150 
cout << a;  // 10

is . clear ();
is . str ( " 1234" );
is >> b; // b = 1234, is . fail() = 0
is . clear ();
is . str ( " 12 34" );
is >> b; // b = 12, is . fail() = 0
is . clear ();
is . str ( "999z" );
is >> b; // b = 999, is . fail() = 0
is . clear ();
is . str ( "abcd" );
is >> b; // b = 999, is . fail() = 1

	for(int x = 0; x < 200; x++)
	{
		for(int y = 199; y >= 0; y--)
		{
			ostringstream o;
			ostringstream p;
			CBigInt a(-x);
			CBigInt b(-y);
			a += b;
			o << a;
			p << (-x + -y);

			if(o.str() != p.str())
			{
				cout << x << " + " << y << endl;
			}
		}
	}

	return 0;
}
#endif
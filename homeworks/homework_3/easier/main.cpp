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
//#define M_PI 3.141592653589
#else
#define debug()
#endif

class CCoord
{
public:
	CCoord(double lat, double lon) : mLat(lat), mLon(lon) { mLatA = fabs(lat); mLonA = fabs(lon); mLatSig = lat < 0.0 ? 'S' : 'N'; mLonSig = lon < 0.0 ? 'W' : 'E'; };
	CCoord(const char* str)
	{
		ParseLine(str, mLat, mLon);
		mLatA = fabs(mLat); 
		mLonA = fabs(mLon);
		mLatSig = mLat < 0.0 ? 'S' : 'N'; 
		mLonSig = mLon < 0.0 ? 'W' : 'E';
	}

	CCoord(const string& str)
	{
		ParseLine(str, mLat, mLon);
		mLatA = fabs(mLat); 
		mLonA = fabs(mLon);
		mLatSig = mLat < 0.0 ? 'S' : 'N'; 
		mLonSig = mLon < 0.0 ? 'W' : 'E';
	}


	static string DToS(const double& val)
	{
		ostringstream os;
		os << val;
		int dot = os.str().find('.');
		int len = os.str().size();
		if(len - dot > 4)
		{
			ostringstream a;
			a << setprecision(3 + dot) << val;
			return a.str();
		}
		return os.str();
	}

	static string DToS2(const double& val)
	{
		ostringstream os;
		double bug = fabs(val - (long)val);
		if(fabs(bug - 1.0) < 0.000001)
		{
			os << val;
			int dot = os.str().find('.');
			string a = os.str();
			ostringstream ret;
			ret << setprecision(dot) << val;
			return ret.str();
		}
		os << (long)val;
		return os.str();
	}

	inline friend ostream& operator<<(ostream& io, const CCoord& c)
	{ 
		io << '(' << (long) c.mLatA << ' ' << DToS2((double)((c.mLatA - (long) c.mLatA) * 60)) << '\'' << DToS(60 *((c.mLat * 60) - (long)(c.mLat * 60))) << '\"' << c.mLatSig
			<< ", " << (long) c.mLonA << ' ' << DToS2((double)((c.mLonA - (long) c.mLonA) * 60)) << '\'' << DToS(60 *((c.mLonA * 60) - (long)(c.mLonA * 60))) << '\"' << c.mLonSig << ')';
		return io;
	}

	inline double Dist(const CCoord& next) const
	{
		//double dLat = (next.mLat - mLat) * M_PI / 180;
		//double dLon = (next.mLon - mLon) * M_PI / 180;
		double lat1 = mLat * M_PI / 180;
		double lat2 = next.mLat * M_PI / 180;
		double lon1 = mLon * M_PI / 180;
		double lon2 = next.mLon * M_PI / 180;

		//double a = sin(dLat/2) * sin(dLat/2) + sin(dLon/2) * sin(dLon/2) * cos(lat1) * cos(lat2); 
		
		return acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon1 - lon2)) * mR;

		//return mR * 2 * atan2(sqrt(a), sqrt(1-a)); ;
	}

	inline static bool ParseLine(const string& in, double& lat, double& lon)
	{
		size_t undef = string::npos;
		size_t lbrace = in.find('(');
		size_t rbrace = in.find(')');
		size_t delim = in.find(',');
		if(lbrace == undef || rbrace == undef || delim == undef) { debug(); return false; }
		bool a = Parse(in.substr(lbrace + 1, delim - lbrace - 1), lat, true);
		bool b = Parse(in.substr(delim + 1, in.size() - delim), lon, false);
		if(!a || !b) { debug(); return false; }

		return true;
	}

	inline static bool Parse(const string& str, double& value, bool lat = true)
	{
		const int undef = -1;
		string input = str;
		if(!lat) replace(input.begin(), input.end(), 'E', 'I');
	
		//cout << ">" << input << "<" << endl;
		istringstream in(input);
		double total = 0;
		char dummy;

		if(input.find('\'') == string::npos)
		{
			if(input.find('.') == string::npos) { debug(); return false; }
			in >> total;
			in >> dummy;
			if(lat)
			{
				if(total > 90) { debug(); return false; }
				if(dummy != 'N' && dummy != 'S') { debug(); return false; }
				if(dummy == 'S') total = -total;
			}
			else
			{
				if(total > 180) { debug(); return false; }
				if(dummy != 'I' && dummy != 'W') { debug(); return false; }
				if(dummy == 'W') total = -total;
			}
		}
		else
		{
			int val = undef;
			in >> val;
			if(val == undef) { debug(); return false; }
			int mid = undef;
			in >> mid;
			if(mid == undef) { debug(); return false; }
			in >> dummy;
			if(dummy != '\'') { debug(); return false; }
			double ter = undef;
			in >> ter;
			if(ter == undef) { debug(); return false; }
			in >> dummy;
			if(dummy != '\"') { debug(); return false; }
			in >> dummy;

			total = (ter / 3600.0) + ((double)mid / 60.0) + val;

			//cout << val << " " << mid << " " << ter  << endl;
			if(lat)
			{
				if(total > 90) { debug(); return false; }
				if(dummy != 'N' && dummy != 'S') { debug(); return false; }
				if(dummy == 'S') total = -total;
			}
			else
			{
				if(total > 180) { debug(); return false; }
				if(dummy != 'I' && dummy != 'W') { debug(); return false; }
				if(dummy == 'W') total = -total;
			}
		}

		value = total;
		return true;
	}

public:
	static const int mR = 6371000;
	double mLat, mLon;
	double mLatA, mLonA;
	char mLatSig, mLonSig;
};

class CGPS
{
public:
	vector<CCoord> data;

	inline CGPS() {};
	inline ~CGPS() {};

	inline friend ostream& operator<<(ostream& os, const CGPS& gps)
	{
		long index = 0;
		os << '[';
		for_each(gps.data.begin(), gps.data.end(), [&index, &os](const CCoord& val) 
		{  
			if(index++ != 0) { os << " > "; }
			os << val;
		});
		os << ']';
		return os;
	}

	inline CGPS& Add(const CCoord& next) 
	{ 
		data.push_back(next); 
		return *this; 
	}

	inline CGPS& operator+=(const CCoord& next) 
	{ 
		data.push_back(next); 
		return *this; 
	}

	inline CCoord& operator[](const int i) 
	{ 
		if(i < (int)data.size() && i >= 0) 
			return data[i]; 
		else throw exception(); 
	}

	inline CGPS operator()(const int s, const int e) const
	{
		if(s <= e && s >= 0 && s < (int)data.size() && e >= 0 && e < (int)data.size())
		{
			CGPS next;
			next.data.insert(next.data.begin(), data.begin() + s, data.begin() + e + 1);
			return next;
		}
		throw exception();
	}

	inline double Distance() const
	{
		double total = 0.0;
		long size = data.size();
		if(size == 0 || size == 1) return 0;

		for(long x = 0; x < size - 1; x++)
		{
			total += data[x].Dist(data[x + 1]);
		}
		return total;
	}

	inline friend istream& operator>>(istream& in, CGPS& gps)
	{
		vector<CCoord> buf;
		double lat, lon;
		string line;
		bool ok = true;
		bool first = true;
		while(getline(in, line, '>'))
		{
			if(first)
			{
				if(line.find('[') == string::npos)
				{
					ok = false;
					break;
				}
				line = line.substr(line.find('[') + 1, line.size());
				first = false;
			}

			int size = line.size();
			int count = 0;
			for(int x = 0; x < size; x++)
			{
				if(line[x] == '[') count++;
			}

			if(count) {ok = false; break; }

			if(!CCoord::ParseLine(line, lat, lon))
			{
				ok = false;
				break;
			}
			buf.push_back(CCoord(lat, lon));
		}
		in.clear();

		if(line.find(']') == string::npos || first)
		{
			ok = false;
		}

		if(ok)
		{
			gps.data.clear();
			gps.data.insert(gps.data.end(), buf.begin(), buf.end());
			in.setstate(ios::eofbit);
		}
		else
		{
			in.setstate(ios::failbit);
		}
		return in;
	}
};

#ifndef __PROGTEST__
int main(int argc, char* argv[])
{
	double d;
	CGPS x0;
	cout << x0 << endl << endl;
	
	istringstream tmp4 ( "[(0N,0E)]" );
	tmp4 >> x0;
	 // tmp4 . fail () = true
	cout << x0 << endl << endl;
	
	return 0;
}

#endif
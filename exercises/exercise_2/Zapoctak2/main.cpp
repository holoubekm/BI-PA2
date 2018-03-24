#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */

class CRef
{
public:
	CRef(): ref(new int(1)) {
		data = new int[1024];
		for(int x = 0; x < 1024; x++) { data[x] = 1024 - x; }
		cout << "alloc" << endl;
	}

	CRef(const CRef& r){
		cout << "copy" << endl;
		ref = r.ref;
		data = r.data;
		Inc();
	}

	void operator=(const CRef& r){
		cout << "assignment" << endl;
		Dec();
		ref = r.ref;
		data = r.data;
		Inc();
	}

	int* data;

	~CRef(){
		Dec();
	}

	friend ostream& operator<<(ostream& os, const CRef& r){
		os << r.data[0] << endl;
		return os;
	}
private:
	void Dec() {
		if(--(*ref) == 0){
			delete data;
			delete ref;
			cout << "deleted" << endl;
		}
	}

	void Inc() {
		(*ref)++;
	}

	int* ref;
};

int main()
{
	CRef* e = new CRef();
	CRef* f = new CRef();
	*f = *e;
	delete e;
	cout << *f << endl;
	delete f;
	system("pause");
}
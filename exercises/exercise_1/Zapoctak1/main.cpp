#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <climits>
#include <string>
#include <fstream>
using namespace std;

template<typename T>
class Set
{
public:
	class Elem{
		public:
			Elem(const T& data) : mNext(nullptr), mData(data), mRef(1){
			}

			Elem(const Elem& ref) : mNext(nullptr), mData(ref.mData), mRef(1){
			}

			void Inc(){
				mRef++;
				cout << "inc: " << mRef << ": " << mData << endl;
			}

			void Dec(){
				cout << "dec: " << mRef << ": " << mData << endl;
				mRef--;
			}

			bool Unique(){
				return mRef <= 1;
			}

		public:
			Elem* mNext;
			T mData;
			int mRef;
	};


	Set() : mCount(0), mHead(nullptr), mLast(nullptr){
	}

	Set(const Set& ref){
		//if(*ref == this) return;

		//ReleaseData();
		mHead = ref.mHead;
		mLast = ref.mLast;
		mCount = ref.mCount;
		Elem* buf = mHead;
		for(int x = 0; x < mCount; x++){
			buf->Inc();
			buf = buf->mNext;
		}
	}

	void PushBack(const T& data){
		mCount++;
		Elem* newOne = new Elem(data);
		if(mHead == nullptr){
			mHead = newOne;
			mLast = newOne;
			return;
		}

		if(mLast->Unique()){
			mLast->mNext = newOne;
			mLast = newOne;
		}
		else{
			mLast->Dec();
			mLast = new Elem(*mLast);
			mLast->mNext = newOne;
			mLast = newOne;
		}
	}

	int Size() const{
		return mCount;
	}

	T& PopBack(){
		if(mCount == 0){
			throw "error";
		}

		Elem* buf = mLast;
		mLast = mHead;
		if(mCount != 1){
			for(int x = 0; x < mCount - 2; x++){
				mLast = mLast->mNext;
			} mLast->mNext = nullptr;
		} else mHead = mLast = nullptr;
		
		buf->Dec();
		mCount--;
		return buf->mData;
	}

	friend ostream& operator<<(ostream& os, const Set& ref){
		Elem* buf = ref.mHead;
		for(int x = 0; x < ref.mCount - 1; x++) {
			cout << buf->mData << " - ";
			buf = buf->mNext;
		} cout << buf->mData;
		return os;
	}

private:
	int mCount;
	Elem* mHead;
	Elem* mLast;
};


int main(int argc, char*argv[]){
	Set<int> a;
	a.PushBack(1);
	a.PushBack(2);
	a.PushBack(3);
	a.PushBack(4);
	a.PushBack(5);
	//Set<int> b = a;
	Set<int>d(a);
	d.PushBack(4);
	cout << a << endl;
	cout << d << endl;
	system("pause");
	return 0;
}
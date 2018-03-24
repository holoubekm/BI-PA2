#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
#include <stdint.h>
using namespace std;
#endif /* __PROGTEST__ */

#define ullong unsigned long long int
#define ulong unsigned long
#define uchar unsigned char

void Error(int line)
{
	#ifndef __PROGTEST__
	cout << line << endl;
	#endif /* __PROGTEST__ */
}

struct utf8
{
	ullong value;
	uchar* data;
	size_t count;
	size_t size;
	vector<bool> bits;
};

struct Node
{
	Node* left;
	Node* right;
	Node* parent;
	utf8* sign;
	size_t count;
	bool isEnd;

	Node()
	{
		left = right = parent = NULL;
		sign = NULL;
		isEnd = false;
		count = 0;
	}
};

bool compare_nodes(const Node* lhs, const Node* rhs) 
{
	return lhs->count > rhs->count;
}


/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

class HuffmanCmp
{
private:
	vector<ullong> signs;
	map<ullong, utf8*> stats;
	Node** table;
	Node* tree;
	uchar* memory;
	uchar curByte;
	ulong byteSize;
	ulong bitSize;
	ulong bytePos;
	ulong bitPos;
	ulong signSize;

	ullong end;

	uchar byteToWrite;
	uchar bitsWritten;
	uchar bytesWritten;

	ofstream output;
	const char* outFile;

public:
	HuffmanCmp(const char* _outFile, uchar* _memory, ulong _byteSize) : memory(_memory), byteSize(_byteSize), outFile(_outFile)
	{
		bitSize = byteSize * 8;
		bitPos = 8;
		bytePos = -1;
		signSize = 0;

		byteToWrite = 0;
		bitsWritten = 0;
		bytesWritten = 0;

		end = (ullong)0xF4908080ULL;

		tree = NULL;
	}

	~HuffmanCmp();
	bool CompressAndSave();
private:
	bool BuildTree();
	void ReleaseTree(Node* node);
	bool GenStatistics();
	void AddUTF8ToMap(utf8* sign);
	bool LoadUTF8Char(utf8*& out);
	void SaveTreeRec(Node* node);
	bool WriteFile();
	void BuildTreeBits(Node* node, vector<bool> bits);
	bool FindAndWrite(ullong value);
	void WriteUchar(uchar data);
	void WriteBit(bool bit);
	bool CanRead();
	bool GetNextBit();
};

HuffmanCmp::~HuffmanCmp()
{
	//ReleaseTree(tree);
	//delete[] memory;
}

bool HuffmanCmp::FindAndWrite(ullong value)
{
	if (stats.count(value) == 0)
	{
		Error(__LINE__);
		return false;
	}

	vector<bool>& bits = stats[value]->bits;
	vector<bool>::iterator it;
	for (it = bits.begin(); it != bits.end(); ++it)
	{
		WriteBit(*it);
	}

	return true;
}

bool HuffmanCmp::WriteFile()
{
	vector<ullong>::iterator it;
	for(it = signs.begin(); it != signs.end(); ++it)
	{
		if (!FindAndWrite(*it))
		{
			Error(__LINE__);
			return false;
		}
	}


	/*
	while (LoadUTF8Char(cur))
	{
		if (cur == NULL)
		{
			//End reached
			isOk = true;
			break;
		}
		
		if (!FindAndWrite(cur->value))
		{
			Error(__LINE__);
			return false;
		}

		delete[] cur->data;
		delete cur;
	}

	if (!isOk)
	{
		Error(__LINE__);
		return false;
	}*/

	//cout << endl << "end: " << endl;
	if (!FindAndWrite(end))
	{
		Error(__LINE__);
		return false;
	}

	//cout << endl << "sub: " << endl;
	while (bitsWritten % 8 != 0)
	{
		WriteBit(false);
	}

	return true;
}

bool HuffmanCmp::CompressAndSave()
{
	if(!BuildTree())
	{
		Error(__LINE__);
		return false;
	}

	output.open(outFile, ios::binary);
	if(!output.is_open())
	{
		Error(__LINE__);
		return false;
	}

	BuildTreeBits(tree, vector<bool>());
	SaveTreeRec(tree);
	if (!WriteFile())
	{
		Error(__LINE__);
		return false;
	}

	if (output.fail())
	{
		Error(__LINE__);
		return false;
	}
	output.close();

	return true;
}

void HuffmanCmp::BuildTreeBits(Node* node, vector<bool> bits)
{
	if (node == NULL) return;

	//cout << node->count << " sign: " << (node->sign != NULL) << endl;
	if (node->sign == NULL)
	{
		vector<bool> right(bits);
		right.push_back(false);
		BuildTreeBits(node->right, right);

		vector<bool> left(bits);
		left.push_back(true);
		BuildTreeBits(node->left, left);
	}
	else
	{
		node->sign->bits = bits;
		//cout << endl;
	}
}

void HuffmanCmp::SaveTreeRec(Node* node)
{
	if(node == NULL) return;

	//cout << endl << "tree: " << endl;
	if(node->sign == NULL)
	{
		WriteBit(false);
	}
	else
	{
		WriteBit(true);
		//cout << endl << "sign: " << endl;
		//cout << node->sign->size << node->sign->value << endl;
		for(size_t x = 0; x < node->sign->size; x++)
		{
			WriteUchar(node->sign->data[x]);
		}
	}

	SaveTreeRec(node->right);
	SaveTreeRec(node->left);
}

void HuffmanCmp::WriteUchar(uchar data)
{
	for(int x = 0; x < 8; x++)
	{
		bool bit = (data >> (7 - x)) & 0x1;
		WriteBit(bit);
	}
}

void HuffmanCmp::WriteBit(bool bit)
{
	//cout << bit << " ";
	byteToWrite |= (bit << (7 - bitsWritten));
	if(++bitsWritten == 8)
	{
		//cout << endl;
		output.write((const char*) &byteToWrite, 1);
		byteToWrite = 0;
		bytesWritten++;
		bitsWritten = 0;
	}
}

bool HuffmanCmp::BuildTree()
{
	if(!GenStatistics())
	{
		Error(__LINE__);
		return false;
	}

	signSize = stats.size();
	if(signSize == 0)
	{
		Error(__LINE__);
		return false;
	}

	size_t pos = 0;
	table = new Node*[signSize];
	for(map<ullong, utf8*>::iterator it = stats.begin(); it != stats.end(); ++it) 
	{
		if(it->second != NULL)
		{
			table[pos] = new Node();
			table[pos]->sign = it->second;
			table[pos]->count = it->second->count;
			pos++;
		}
	}
	stable_sort(table, table + signSize, compare_nodes);


	for(size_t x = signSize - 1; x > 0; x--)
	{
		Node* parent = new Node();
		Node* left = table[x - 1];
		Node* right = table[x];

		parent->left = left;
		left->parent = parent;
		parent->right = right;
		right->parent = parent;
		parent->count = left->count + right->count;
		table[x - 1] = parent;

		//sort(table, table + x + 1, compare_nodes);

		
		size_t y = x - 1;
		while(y > 0 && table[y]->count > table[y - 1]->count)
		{
			Node* buf = table[y];
			table[y] = table[y - 1];
			table[y - 1] = buf;
			y--;
		}
	}

	tree = table[0];




	//delete[] table;

	return true;
}

void HuffmanCmp::ReleaseTree(Node* node)
{
	if (node != NULL)
	{
		ReleaseTree(node->left);
		ReleaseTree(node->right);
		
		if (node->sign != NULL)
		{
			delete[] node->sign->data;
			delete node->sign;
		}

		delete node;
	}
}

void HuffmanCmp::AddUTF8ToMap(utf8* sign)
{
	bool contains = stats.count(sign->value) != 0;

	if(contains)
	{
		stats[sign->value]->count++;
		//delete[] sign->data;
		//delete sign;
	}
	else
	{
		sign->count++;
		stats[sign->value] = sign;
	}
}

bool HuffmanCmp::GenStatistics()
{
	utf8* cur = NULL;
	bool isOk = false;
	while(LoadUTF8Char(cur))
	{
		if(cur == NULL)
		{
			//End reached
			isOk = true;
			break;
		}
		signs.push_back(cur->value);
		AddUTF8ToMap(cur);
	}

	if(!isOk)
	{
		Error(__LINE__);
		return false;
	}
	else
	{
		utf8* endSign = new utf8();
		endSign->size = 4;
		endSign->data = new uchar[4];
		endSign->data[0] = 0xF4;
		endSign->data[1] = 0x90;
		endSign->data[2] = 0x80;
		endSign->data[3] = 0x80;
		endSign->count = 0;
		endSign->value = end;
		AddUTF8ToMap(endSign);
	}

	return true;
}

bool HuffmanCmp::LoadUTF8Char(utf8*& out)
{
	if(!CanRead())
	{
		out = NULL;
		return true;
	}

	uchar* data = NULL;
	int size = 0;

	bool byte[8];
	for (int x = 0; x < 8; x++)
	{
		if (CanRead())
		{
			byte[x] = GetNextBit();
		}
		else
		{	
			out = NULL;
			return false;
		}
	}


	if (!byte[0])
	{
		size = 1;
	}
	else if (byte[0] && byte[1] && !byte[2])
	{
		size = 2;
	}
	else if (byte[0] && byte[1] && byte[2] && !byte[3])
	{
		size = 3;
	}
	else if (byte[0] && byte[1] && byte[2] && byte[3] && !byte[4])
	{
		size = 4;
	}/*
	else if (byte[0] && byte[1] && byte[2] && byte[3] && byte[4] && !byte[5])
	{
		size = 5;
	}
	else if (byte[0] && byte[1] && byte[2] && byte[3] && byte[4] && byte[5] && !byte[6])
	{
		size = 6;
	}*/
	else
	{
		Error(__LINE__);
		out = NULL;
		return false;
	}

	data = new uchar[size];
	
	for (int x = 0; x < size; x++)
	{
		data[x] = 0;
	}

	for (int x = 0; x < 8; x++)
	{
		data[0] |= (byte[x] << (7 - x));
	}
	
	//cout << data[0] << endl;
	
	for (int x = 1; x < size; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if (CanRead())
			{
				byte[y] = GetNextBit();
			}
			else
			{
				delete[] data;
				Error(__LINE__);
				out = NULL;
				return false;
			}
		}

		if (!byte[0] || byte[1])
		{
			delete[] data;
			Error(__LINE__);
			out = NULL;
			return false;
		}
		for (int y = 0; y < 8; y++)
		{
			data[x] |= (byte[y] << (7 - y));
		}
	}

	ullong value = 0;
	for(int x = 0; x < size; x++)
	{
		value |= ((ullong)data[x]) << ((size - x - 1) * 8);
		//cout << "Hodnota utf8 je: " << value << endl;
	}

	utf8* sign = new utf8();
	sign->data = data;
	sign->size = size;
	sign->value = value;
	sign->count = 0;
	out = sign;

	return true;
}

bool HuffmanCmp::CanRead()
{
	return (bytePos * 8 + bitPos) < bitSize;
}

bool HuffmanCmp::GetNextBit()
{
	////Musime kricet pri prekroceni pole!

	if (bitPos == 8)
	{
		bytePos++;
		bitPos = 0;

		curByte = memory[bytePos];
	}

	bool value = (curByte >> (0x7 - bitPos)) & 0x1;
	bitPos++;

	return value;
}

bool compressFile(const char * inFile, const char * outFile)
{
	ifstream check(outFile, ios::binary);
	if (check.is_open())
	{
		Error(__LINE__);
		return false;
	}
	check.close();


	ifstream file(inFile, ifstream::binary | ios::ate);
	if (file.is_open())
	{
		size_t size = (size_t)file.tellg();
		char* memblock = new char[(unsigned int)size];
		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();


		if (file.fail())
		{
			delete[] memblock;
			Error(__LINE__);
			return false;
		}


		ulong byteSize = (ulong)size;
		uchar* memory = new uchar[byteSize];
		for (ulong x = 0; x < byteSize; x++)
		{
			memory[x] = (uchar)memblock[x];
		}
		delete[] memblock;

		if (byteSize == 0)
		{
			Error(__LINE__);
			return false;
		}

		HuffmanCmp compressor(outFile, memory, size);
		if (!compressor.CompressAndSave())
		{
			Error(__LINE__);
			return false;
		}
	}
	else
	{
		Error(__LINE__);
		return false;
	}

	return true;
}

/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

class HuffmanDec
{
private:
	uchar curByte;
	uchar* memory;
	ulong byteSize;
	ulong bitSize;
	ulong bytePos;
	ulong bitPos;
	Node* tree;
	Node* endNode;
	const char* outFile;
	bool recOk;

public:
	HuffmanDec(const char* _outFile, uchar* _memory, ulong _byteSize) : memory(_memory), byteSize(_byteSize), outFile(_outFile)
	{
		bitSize = byteSize * 8;
		bitPos = 8;
		bytePos = -1;
		tree = NULL;
		endNode = NULL;
		recOk = true;
	}
	~HuffmanDec();
	bool BuildTree();
	bool DecompressAndSave();

private:
	bool CanRead();
	bool GetNextBit();
	utf8* LoadUTF8Char();
	Node* RecBuildNodes();
	void ReleaseTree(Node* node);
};

HuffmanDec::~HuffmanDec()
{
	ReleaseTree(tree);
	delete[] memory;
}

bool HuffmanDec::CanRead()
{
	return (bytePos * 8 + bitPos) < bitSize;
}

bool HuffmanDec::GetNextBit()
{
	////Musime kricet pri prekroceni pole!

	if (bitPos == 8)
	{
		bytePos++;
		bitPos = 0;

		curByte = memory[bytePos];
	}

	bool value = (curByte >> (0x7 - bitPos)) & 0x1;
	bitPos++;

	return value;
}

utf8* HuffmanDec::LoadUTF8Char()
{
	uchar* data = NULL;
	int size = 0;

	bool byte[8];
	for (int x = 0; x < 8; x++)
	{
		if (CanRead())
		{
			byte[x] = GetNextBit();
		}
		else
		{
			return NULL;
		}
	}


	if (!byte[0])
	{
		size = 1;
	}
	else if (byte[0] && byte[1] && !byte[2])
	{
		size = 2;
	}
	else if (byte[0] && byte[1] && byte[2] && !byte[3])
	{
		size = 3;
	}
	else if (byte[0] && byte[1] && byte[2] && byte[3] && !byte[4])
	{
		size = 4;
	}
	else if (byte[0] && byte[1] && byte[2] && byte[3] && byte[4] && !byte[5])
	{
		size = 5;
	}
	else if (byte[0] && byte[1] && byte[2] && byte[3] && byte[4] && byte[5] && !byte[6])
	{
		size = 6;
	}
	else
	{
		Error(__LINE__);
		return NULL;
	}

	data = new uchar[size];
	
	for (int x = 0; x < size; x++)
	{
		data[x] = 0;
	}

	for (int x = 0; x < 8; x++)
	{
		data[0] |= (byte[x] << (7 - x));
	}
	
	//cout << data[0] << endl;
	
	for (int x = 1; x < size; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			if (CanRead())
			{
				byte[y] = GetNextBit();
			}
			else
			{
				delete[] data;
			}
		}

		if (!byte[0] || byte[1])
		{
			delete[] data;
			Error(__LINE__);
			return NULL;
		}

		//cout << endl;
		for (int y = 0; y < 8; y++)
		{/*
			if (size == 4)
			{
				cout << byte[y] << " ";
			}*/
			data[x] |= (byte[y] << (7 - y));
		}
	}

	utf8* sign = new utf8();
	sign->data = data;
	sign->size = size;
	return sign;
}

Node* HuffmanDec::RecBuildNodes()
{
	if (!CanRead())
	{
		recOk = false;
		Error(__LINE__);
		return NULL;
	}

	bool bit = GetNextBit();
	if (!bit)
	{
		//Make inner node
		Node* innerNode = new Node();
		innerNode->left = RecBuildNodes();
		if (innerNode->left == NULL)
		{
			delete innerNode;
			Error(__LINE__);
			return NULL;
		}

		innerNode->left->parent = innerNode;
		innerNode->right = RecBuildNodes();
		if (innerNode->right == NULL)
		{
			delete innerNode;
			Error(__LINE__);
			return NULL;
		}

		innerNode->right->parent = innerNode;

		return innerNode;
	}
	else
	{
		//Make leaf
		Node* innerLeaf = new Node();
		innerLeaf->sign = LoadUTF8Char();

		if (innerLeaf->sign == NULL)
		{
			recOk = false;
			delete innerLeaf;
			Error(__LINE__);
			return NULL;
		}

		if (innerLeaf->sign->size == 4)
		{
			if (innerLeaf->sign->data[0] == (uchar)0xF4 &&
				innerLeaf->sign->data[1] == (uchar)0x90 &&
				innerLeaf->sign->data[2] == (uchar)0x80 &&
				innerLeaf->sign->data[3] == (uchar)0x80)
			{
				innerLeaf->isEnd = true;
				endNode = innerLeaf;
			}
		}
		return innerLeaf;
	}
}

void HuffmanDec::ReleaseTree(Node* node)
{
	if (node != NULL)
	{
		ReleaseTree(node->left);
		ReleaseTree(node->right);

		if (node->sign != NULL)
		{
			delete[] node->sign->data;
			delete node->sign;
		}
		delete node;
	}
}

bool HuffmanDec::BuildTree()
{
	tree = RecBuildNodes();

	if (!recOk || tree == NULL || endNode == NULL)
	{
		ReleaseTree(tree);
		Error(__LINE__);
		return false;
	}

	return true;
}

bool HuffmanDec::DecompressAndSave()
{
	Node* cur = tree;
	ofstream output(outFile, ofstream::binary);
	if (!output.is_open())
	{
		Error(__LINE__);
		return false;
	}

	bool endReached = false;
	while (CanRead())
	{
		bool bit = GetNextBit();
		if (!bit)
		{
			cur = cur->left;
		}
		else
		{
			cur = cur->right;
		}

		if (cur->sign != NULL)
		{
			if (cur->isEnd)
			{
				endReached = true;
				break;
			}

			output.write((const char*)cur->sign->data, cur->sign->size);

			if (output.fail())
			{
				Error(__LINE__);
				output.close();
				return false;
			}
			cur = tree;
		}
	}

	output.close();
	if(!endReached)
	{
		remove(outFile);
	}

	return endReached;
}

bool decompressFile(const char * inFile, const char * outFile)
{
	streampos size;
	char * memblock;
	
	//Outfile already exists
	ifstream check(outFile);
	if(check.is_open())
	{
		check.close();
		Error(__LINE__);
		return false;
	}

	ifstream file(inFile, ifstream::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[(unsigned int)size];
		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();

		
		if(file.fail())
		{
			delete[] memblock;
			Error(__LINE__);
			return false;
		}
		

		ulong byteSize = (ulong)size;
		uchar* memory = new uchar[byteSize];
		for(ulong x = 0; x < byteSize; x++)
		{
			memory[x] = (uchar)memblock[x];
		}
		delete[] memblock;

		if (byteSize == 0)
		{
			Error(__LINE__);
			return false;
		}

		HuffmanDec decoder(outFile, memory, byteSize);
		if (!decoder.BuildTree())
		{
			Error(__LINE__);
			return false;
		}

		if (!decoder.DecompressAndSave())
		{
			Error(__LINE__);
			return false;
		}
	}
	else
	{
		Error(__LINE__);
		return false;
	}

	return true;
}

#ifndef __PROGTEST__
int main(int argc, char * argv[])
{
	/*
	ulong size = 5;
	uchar* memory = new uchar[size];
	memory[0] = 0x5A; // sign
	memory[1] = 0xC6;
	memory[2] = 0xAD; // sign
	memory[3] = 0x06; // sign
	memory[4] = 0x06; // sign
	*/
	/*
	
	ulong size = 8;
	uchar* memory = new uchar[size];
	memory[0] = 0x4B; // K sign
	memory[1] = 0x6F; // O sign
	memory[2] = 0x6C; // L sign
	memory[3] = 0x6F; // O sign
	memory[4] = 0x74; // T sign
	memory[5] = 0x6F; // O sign
	memory[6] = 0xC4; 
	memory[7] = 0x8D; // Č
	*/

	/*
	ulong size = 10;
	uchar* memory = new uchar[size];
	memory[0] = 0x61; // K sign
	memory[1] = 0x61; // O sign
	memory[2] = 0x61; // L sign
	memory[3] = 0x61; // O sign
	memory[4] = 0x62; // T sign
	memory[5] = 0x62; // O sign
	memory[6] = 0x62;
	memory[7] = 0x63; // Č
	memory[8] = 0x63; // Č
	memory[9] = 0x64; // Č
	*/

	ifstream input("input.txt", ifstream::binary | ios::ate);
	if (!input.is_open())
	{
		Error(__LINE__);
		return false;
	}
	size_t size = (size_t)input.tellg();
	char* memblock = new char[(unsigned int)size];
	input.seekg(0, ios::beg);
	input.read(memblock, size);
	input.close();

	ulong byteSize = (ulong)size;
	uchar* memory = new uchar[byteSize];
	for (ulong x = 0; x < byteSize; x++)
	{
		memory[x] = (uchar)memblock[x];
	}
	delete[] memblock;
	HuffmanCmp compressor("output.bin", memory, size);
	compressor.CompressAndSave();
	

	/*************************************/


	ifstream file("output.bin", ifstream::binary | ios::ate);
	if (!file.is_open())
	{
		Error(__LINE__);
		return false;
	}
	size = (size_t)file.tellg();
	memblock = new char[(unsigned int)size];
	file.seekg(0, ios::beg);
	file.read(memblock, size);
	file.close();
	
	byteSize = (ulong)size;
	uchar* memory2 = new uchar[byteSize];
	for (ulong x = 0; x < byteSize; x++)
	{
		memory2[x] = (uchar)memblock[x];
	}
	delete[] memblock;
	HuffmanDec decoder("hotovo.txt", memory2, byteSize);
	decoder.BuildTree();
	decoder.DecompressAndSave();
	/*
	string fileName;
	cin >> fileName;

	decompressFile(fileName.c_str(), "output.txt");
	*/
	//BuildTree();
	// tests
#ifdef _WIN32
	system("pause");
#endif
	return 0;
}
#endif /* __PROGTEST__ */
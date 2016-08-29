#pragma once
#include "huffman_tree.h"
#include<algorithm>  
#include<windows.h>  
#include<string>

struct FileInfo
{
	unsigned char _ch;
	int  _count;
	string     _code;

	FileInfo(unsigned char ch = 0)
		:_ch(ch)
		,_count(0)
	{}
		
	FileInfo operator + (FileInfo& f)
	{
		FileInfo tmp;
		tmp._count = this->_count + f._count;
		return tmp;
	}

	bool operator < (FileInfo& f)
	{
		return this->_count < f._count;
	}
	bool operator != (const FileInfo& f)const  
    {  
        return this->_count != f._count;  
    }  
};


class FileCompress
{
public:
	FileCompress()
	{
		for(int i = 0; i < 256; i++)
			_info[i]._ch = i;
	}

	bool Compress(const char* filename)
	{
		long long CharCount = 0;
		assert(filename);
		FILE* fOut = fopen(filename,"rb");

		assert(fOut);
		
		char ch = fgetc(fOut);
		//统计文件中各字符出现的次数，及总字符数
		while(ch != EOF)
		{
			_info[(unsigned char)ch]._count ++;
			ch = fgetc(fOut);
			CharCount++;
		}
		//生成huffmancode
		GenerateHuffmanCode();

		string CompressFile = filename;
		CompressFile += ".compress";
		FILE* fCompress = fopen(CompressFile.c_str(),"wb");
		assert(fCompress);

		fseek(fOut, 0, SEEK_SET);//置文件指针在文件头
		ch = fgetc(fOut);
		unsigned char inch = 0;
		int index = 0;
		while(ch != EOF)
		{
			string& code = _info[(unsigned char)ch]._code;
			for(size_t i = 0; i<code.size();++i)
			{
				inch = inch << 1;
				if(code[i] == '1')
					inch |= 1;
				if(++index == 8)
				{
					fputc(inch,fCompress);
					inch = 0;
					index = 0;
				}
			}
			ch = fgetc(fOut);
		}

		if(index)
		{
			inch = inch << (8 - index);
			fputc(inch,fCompress);
		}

		//配置文件(解压缩要用！！！)
		string ConfigFile = filename;
		ConfigFile += ".config";
		FILE* fConfig = fopen(ConfigFile.c_str(),"wb");
		assert(fConfig);

		char CountStr[128];
		itoa(CharCount >> 32, CountStr,10);
		fputs(CountStr,fConfig);
		fputc('\n', fConfig);
		itoa(CharCount & 0xffffffff, CountStr, 10);
		fputs(CountStr, fConfig);
		fputc('\n', fConfig);

		for(int i = 0; i < 256 ; ++i)
		{
				fputc(_info[i]._ch, fConfig);
				fputc(',', fConfig);
				//fputc(_info[i]._count +'0', fConfig);
				itoa(_info[i]._count,CountStr, 10);
				fputs(CountStr , fConfig);
				fputc('\n', fConfig);
				
		}

		fclose(fOut);
		fclose(fCompress);
		fclose(fConfig);
		return true;
	}

	bool unCompress(const char* filename)
	{
		string ConfigFile = filename;
		ConfigFile += ".config";
		FILE* outConfig = fopen(ConfigFile.c_str(), "rb");
		assert(outConfig);

		char ch;
		long long CharCount = 0;
		string line ;
		ReadLine(outConfig,line);
		CharCount = atoi(line.c_str());	
		CharCount <<= 32;
		line.clear();
		ReadLine(outConfig,line);
		CharCount += atoi(line.c_str());
		line.clear();

		while(ReadLine(outConfig,line))
		{
			if(!line.empty())
			{
				ch = line[0];
				_info[(unsigned char)ch]._count = atoi(line.substr(2).c_str());		
				line.clear();
			}
			else
			{
				line = '\n';
			}
		}

		HuffmanTree<FileInfo> t;
		FileInfo invalid;  
        t.CreateTree(_info, 256, invalid);  
		HuffmanTreeNode<FileInfo>* root = t.GetRoot();

		string unCompressFile = filename;
		unCompressFile += ".uncompress";
		FILE* fOut = fopen(unCompressFile.c_str(),"wb");
		assert(fOut);

		string CompressFile = filename;
		CompressFile += ".compress";
		FILE* fIn = fopen(CompressFile.c_str(),"rb");
		assert(fIn);

		int pos = 8;
		HuffmanTreeNode<FileInfo>* cur = root;
		ch = fgetc(fIn);

		while((unsigned char)ch != EOF && CharCount)
		{
			--pos;
			if((unsigned char)ch & (1 << pos))
				cur = cur->_right;
			else
				cur = cur->_left;
			if(cur->_left == NULL && cur->_right == NULL)
			{
				fputc(cur->_weight._ch, fOut);
				cur = root;
				--CharCount;
			}
			if(pos == 0)
			{
				ch = fgetc(fIn);
				pos = 8;
			}
		}

		fclose(outConfig);
		fclose(fIn);
		fclose(fOut);
		return true;
	}
protected:
	bool ReadLine(FILE *fOut,string &str)
	{
		char ch =fgetc(fOut);
		if(ch == EOF)
			return false;
		while(ch != EOF && ch != '\n')
		{
			str += ch;
			ch = fgetc(fOut);
		}
		return true;
	}
	void GenerateHuffmanCode()
	{
		HuffmanTree<FileInfo> t;
		FileInfo invalid;  
        t.CreateTree(_info, 256, invalid);  
		_GenerateHuffmanCode(t.GetRoot());
	}
	void _GenerateHuffmanCode(HuffmanTreeNode<FileInfo>*  root)
	{
		if(root == NULL)
		{
			return;
		}
		_GenerateHuffmanCode(root->_left);
		_GenerateHuffmanCode(root->_right);
		if(root->_left == NULL && root->_right == NULL)
		{
			HuffmanTreeNode<FileInfo>* cur = root;
			HuffmanTreeNode<FileInfo>* parent = cur->_parent;
			string& code = _info[cur->_weight._ch]._code;
			while(parent)
			{
				if(parent->_left == cur)
					code += '0';
				else if(parent->_right == cur)
					code += '1';
				cur = parent;
				parent =cur->_parent;
			}
			reverse(code.begin(),code.end());
		}
	}
private:
	FileInfo _info[256];
};
void TestFileCompress()  
{  
  
    FileCompress f;  
  
    int begin1 = GetTickCount();  
    f.Compress("testing.txt");  
    int end1 = GetTickCount();  
    cout << end1 - begin1 << endl;  
 //   int begin2 = GetTickCount();  
 //   f.unCompress("testing.txt");  
 //   int end2 = GetTickCount();  
 //   cout << end2 - begin2 << endl;  
  
}
void TestFileunCompress()  
{
	FileCompress f;
    int begin2 = GetTickCount();  
    f.unCompress("testing.txt");  
    int end2 = GetTickCount();  
    cout << end2 - begin2 << endl;  
}

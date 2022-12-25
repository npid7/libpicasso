#pragma once
#include <stdio.h>
#include "picasso/types.h"
#include <sstream>
#include <string>

class FileClass
{
	std::stringstream f;
	bool LittleEndian, own;
	int filePos;

	size_t _RawRead(void* buffer, size_t size)
	{
        f.read((char*)buffer, size);
		filePos += size;
		return size;
	}

	size_t _RawWrite(const void* buffer, size_t size)
	{
		f.write((const char*)buffer, size);
		filePos += size;
		return size;
	}

public:
	FileClass(const char* file, const char* mode) : LittleEndian(true), own(true), filePos(0)
	{
		//Do nothing
	}
	~FileClass()
	{
		//Do nothing
	}

	void SetLittleEndian() { LittleEndian = true; }
	void SetBigEndian() { LittleEndian = false; }

	std::stringstream* get_ptr() { return &f; }
	bool openerror() { return false; }

	dword_t ReadDword()
	{
		dword_t value;
		_RawRead(&value, sizeof(dword_t));
		return LittleEndian ? le_dword(value) : be_dword(value);
	}

	void WriteDword(dword_t value)
	{
		value = LittleEndian ? le_dword(value) : be_dword(value);
		_RawWrite(&value, sizeof(dword_t));
	}

	word_t ReadWord()
	{
		word_t value;
		_RawRead(&value, sizeof(word_t));
		return LittleEndian ? le_word(value) : be_word(value);
	}

	void WriteWord(word_t value)
	{
		value = LittleEndian ? le_word(value) : be_word(value);
		_RawWrite(&value, sizeof(word_t));
	}

	hword_t ReadHword()
	{
		hword_t value;
		_RawRead(&value, sizeof(hword_t));
		return LittleEndian ? le_hword(value) : be_hword(value);
	}

	void WriteHword(hword_t value)
	{
		value = LittleEndian ? le_hword(value) : be_hword(value);
		_RawWrite(&value, sizeof(hword_t));
	}

	byte_t ReadByte()
	{
		byte_t value;
		_RawRead(&value, sizeof(byte_t));
		return value;
	}

	void WriteByte(byte_t value)
	{
		_RawWrite(&value, sizeof(byte_t));
	}

	float ReadFloat()
	{
		union { word_t w; float f; } t;
		t.w = ReadWord();
		return t.f;
	}

	void WriteFloat(float value)
	{
		union { word_t w; float f; } t;
		t.f = value;
		WriteWord(t.w);
	}

	bool ReadRaw(void* buffer, size_t size) { return _RawRead(buffer, size) == size; }
	bool WriteRaw(const void* buffer, size_t size) { return _RawWrite(buffer, size) == size; }

	int Tell() { return filePos /*ftell(f)*/; }
};

static inline char* StringFromFile(const char* filename)
{
	FILE* f = fopen(filename, "rb");
	if (!f) return NULL;
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	rewind(f);
	char* buf = (char*)malloc(size+1);
	if (!buf)
	{
		fclose(f);
		return NULL;
	}
	fread(buf, 1, size, f);
	buf[size] = 0;
	fclose(f);
	return buf;
}
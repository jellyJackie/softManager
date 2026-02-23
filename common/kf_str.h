#pragma once

#include <iosfwd>
#include <string>
#include <vector>

class KfString {
public:
	KfString();

	KfString(const char* value, size_t len);

	KfString(const char* value); 

	KfString(const wchar_t* value);

	KfString(const wchar_t* value, size_t len);

	KfString(const KfString& other);

	KfString(KfString&& other) noexcept;

	~KfString();

	KfString& operator=(const char* value);

	KfString& operator=(const wchar_t* value);

	KfString& operator=(const KfString& other);

	KfString& operator=(KfString&& other) noexcept;

	bool operator==(const char* value) const;

	bool operator==(const wchar_t* value) const;

	bool operator==(const KfString& value) const;

	bool CompareNoCase(const char* value) const;

	bool CompareNoCase(const wchar_t* value) const;

	bool CompareNoCase(const KfString& value) const;

	KfString operator+(const char* value) const;

	KfString operator+(const wchar_t* value) const;

	KfString operator+(const KfString& other) const;

	KfString& operator+=(const char* value);

	KfString& operator+=(const wchar_t* value);

	KfString& operator+=(const KfString& value);

	char operator[](int index) const;

	operator const char* () const;

	operator std::wstring() const;

	KfString& MakeUpper();

	KfString& MakeLower();

	KfString& Trim(char filter = ' ');

	KfString& AppendFormat(const char* const format, ...);

	KfString& AppendFormat(const wchar_t* const format, ...);

	KfString& Append(const char* value);

	KfString& Append(const wchar_t* value);

	KfString& Append(const KfString& value);

	KfString& Insert(int pos, const char* value);

	KfString& Insert(int pos, const wchar_t* value);

	KfString& Insert(int pos, const KfString& value);

	static KfString Format(const char* const format, ...);

	static KfString Format(const wchar_t* const format, ...);

	static std::string FormatList(const char* format, va_list list);

	static std::wstring FormatList(const wchar_t* format, va_list list);

	size_t Find(const char* filter) const;

	size_t Find(const wchar_t* filter) const;

	size_t Find(char filter) const;

	size_t ReverseFind(const char* filter) const;

	size_t ReverseFind(const wchar_t* filter) const;

	size_t ReverseFind(char filter) const;

	KfString SubStr(int start_pos, size_t length = -1) const;

	KfString Left(int length) const;

	KfString Right(size_t length) const;

	KfString& Replace(const char* src, const char* dest);

	KfString& Replace(const wchar_t* src, const wchar_t* dest);

	size_t GetLength() const;

	uint32_t GetCapacity() const;

	bool IsEmpty() const;

	const char* GetString() const;

	char* GetData() const;

	std::string GetUtf8String();

	std::wstring GetWString() const;

	std::vector<KfString> Split(const char* filter) const;
private:
	void CheckAppendCapacity(size_t append_size);

	void Reallocate(size_t size);

	uint32_t capacity_;
	char* value_;
};

KfString operator+(const char* value, const KfString& other);

std::ostream& operator<<(std::ostream& os, const KfString& other);

void KfStringTest();
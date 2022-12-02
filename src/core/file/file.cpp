#include "file.h"
#include "core/misc/path.h"

namespace luna
{

RegisterType_Imp(LFileInfo, LFileInfo)
{

}

void LFileStream::Write(const byte *Source, int64_t BytesToWrite)
{
	mFileStream.seekp(mPos);
	mFileStream.write((char *)Source, BytesToWrite);
}

void LFileStream::Flush(const bool bFullFlush /*= false*/)
{
	mFileStream.flush();
}

void LFileStream::Read(byte *Destination, int64_t BytesToRead)
{
	mFileStream.seekg(mPos);
	mFileStream.read((char *)Destination, BytesToRead);
}

int64_t LFileStream::Size()
{
	auto pos = mFileStream.tellg();
	mFileStream.seekg(0, std::ios::end);
	auto res = mFileStream.tellg();
	mFileStream.seekg(pos);
	return res;
}

void LFileStream::ReadToVector(LVector<byte> &dest)
{
	auto size = Size();
	dest.reserve(size);
	Read(dest.data(), size);
}

void LFileStream::WriteFromVector(LVector<byte> &dest)
{
	auto size = dest.size();
	Write(dest.data(), size);
}
}
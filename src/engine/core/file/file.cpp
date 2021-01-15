#include "file.h"
#include "core/misc/path.h"


namespace luna
{

void LFileStream::Write(const byte *Source, int64_t BytesToWrite)
{
	m_file.seekp(m_pos);
	m_file.write((char *)Source, BytesToWrite);
}

void LFileStream::Flush(const bool bFullFlush /*= false*/)
{
	m_file.flush();
}

void LFileStream::Read(byte *Destination, int64_t BytesToRead)
{
	m_file.seekg(m_pos);
	m_file.read((char *)Destination, BytesToRead);
}

int64_t LFileStream::Size()
{
	auto pos = m_file.tellg();
	m_file.seekg(0, std::ios::end);
	auto res = m_file.tellg();
	m_file.seekg(pos);
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
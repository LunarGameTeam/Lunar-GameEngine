#include "file.h"
#include "core/misc/path.h"


namespace luna
{

void LFile::Write(const byte *Source, int64_t BytesToWrite)
{
	m_file.seekp(m_pos);
	m_file.write((char *)Source, BytesToWrite);
}

void LFile::Flush(const bool bFullFlush /*= false*/)
{
	m_file.flush();
}

void LFile::Read(byte *Destination, int64_t BytesToRead)
{
	m_file.seekg(m_pos);
	m_file.read((char *)Destination, BytesToRead);
}

int64_t LFile::Size()
{
	auto pos = m_file.tellg();
	m_file.seekg(0, std::ios::end);
	auto res = m_file.tellg();
	m_file.seekg(pos);
	return res;
}

void LFile::ReadToVector(LVector<byte> &dest)
{
	auto size = Size();
	dest.reserve(size);
	Read(dest.data(), size);
}

void LFile::WriteFromVector(LVector<byte> &dest)
{
	auto size = dest.size();
	Write(dest.data(), size);
}


}
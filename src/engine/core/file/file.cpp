#include "file.h"
#include "core/misc/path.h"

using namespace luna;

void LFile::Write(const byte* Source, int64_t BytesToWrite)
{
	m_File.seekp(m_Pos);
	m_File.write((char*)Source, BytesToWrite);
}

void LFile::Flush(const bool bFullFlush /*= false*/)
{
	m_File.flush();
}

void LFile::Read(byte* Destination, int64_t BytesToRead)
{
	m_File.seekg(m_Pos);
	m_File.read((char*)Destination, BytesToRead);
}
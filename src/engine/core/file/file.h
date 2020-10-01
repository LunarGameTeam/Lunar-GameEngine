#pragma once 

#include "core/private_core.h"
#include "core/object/object.h"
#include "path.h"
#include <fstream>

namespace LunarEngine
{
	class IFile
	{
	public:
		bool Ready()
		{
			return m_ready;
		}
		virtual void Read(uint8_t* Destination, int64_t BytesToRead) = 0;
		virtual void Write(const uint8_t* Source, int64_t BytesToWrite) = 0;
		virtual void Flush(const bool bFullFlush = false) = 0;
		virtual int64_t Size();

	private:
		bool m_ready = false;


	};

	class LFile : public Object, public IFile
	{

	public:
		int64_t Tell() const
		{
			return m_Pos;
		}
		void Seek(int64_t pos)
		{
			m_Pos = pos;
		}
		void Read(byte * Destination, int64_t BytesToRead) override;
		void Write(const byte* Source, int64_t BytesToWrite) override;
		void Flush(const bool bFullFlush = false) override;
		int m_Mode;
		int64_t m_Pos;
		std::fstream m_File;
	protected:
		
	};


}
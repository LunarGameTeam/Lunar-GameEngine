#pragma once 

#include "core/private_core.h"
#include "core/object/object.h"
#include "path.h"
#include <fstream>

namespace luna
{
	
	class LFile
	{
	public:
		bool Ready()
		{
			return m_ready;
		}
		int64_t Tell() const
		{
			return m_pos;
		}
		void Seek(int64_t pos)
		{
			m_pos = pos;
		}
		virtual void Read(byte * Destination, int64_t BytesToRead);
		virtual void Write(const byte * Source, int64_t BytesToWrite) ;
		virtual void Flush(const bool bFullFlush = false);
		virtual int64_t Size();


		virtual void ReadToVector(LVector<byte> &dest);
		virtual void WriteFromVector(LVector<byte> &dest);		

	private:
		int m_mode;
		int64_t m_pos;
		std::fstream m_file;
		bool m_ready = false;

		friend class WindowsFileManager;


	};



}
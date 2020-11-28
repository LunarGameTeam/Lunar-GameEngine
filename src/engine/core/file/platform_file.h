#pragma once

#include "core/private_core.h"
#include "path.h"
#include "file.h"

namespace LunarEngine
{
	class IPlatformFileManager
	{
	public:
		virtual bool InitFileManager() = 0;

		virtual LSharedPtr<LFile> Open(const LPath& path,int mode) = 0;
		virtual bool IsExists(const LPath& path) = 0;
		virtual bool IsDirctory(const LPath& path) = 0;
		virtual bool IsFile(const LPath& path) = 0;
		virtual bool DeleteFile(const LPath& path) = 0;
		virtual bool CreateDirectory(const LPath& path) = 0;
		virtual bool CreateFile(const LPath& path) = 0;
		
		virtual const LString& EngineDir() = 0;
	protected:
		LString m_EngineDir;
		LString m_ApplicationPath;
	};

	class WindowsFileManager : public IPlatformFileManager
	{
	public:
		bool InitFileManager()override;

		LSharedPtr<LFile> Open(const LPath& path, int mode) override;
		bool IsExists(const LPath& path) override;
		bool IsDirctory(const LPath& path) override;
		bool IsFile(const LPath& path) override;
		bool DeleteFile(const LPath& path) override;
		bool CreateDirectory(const LPath& path) override;
		bool CreateFile(const LPath& path) override;
		const LString& EngineDir() override;
	};
}
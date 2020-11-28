#include "file_subsystem.h"

using namespace luna;

bool FileSubsystem::OnPreInit()
{

#ifdef _WIN32 || _WIN64 
	m_PlatformFile =  new WindowsFileManager();
	m_PlatformFile->InitFileManager();
#endif
	return true;
}

bool FileSubsystem::OnPostInit()
{
	return true;
}

bool FileSubsystem::OnInit()
{
	return true;
}

bool FileSubsystem::OnShutdown()
{
	delete m_PlatformFile;
	return true;
}

void FileSubsystem::Tick()
{
}

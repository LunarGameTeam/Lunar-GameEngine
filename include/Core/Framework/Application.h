

#include "Core/Foundation/Config.h"

#include "Core/Framework/Module.h"
#include "Core/Framework/LunaCore.h"
#include "Core/Reflection/Reflection.h"

namespace luna
{

class LApplication;

extern CORE_API LApplication* s_app;

class CORE_API LApplication : public LObject
{
	RegisterTypeEmbedd(LApplication, LObject)
public:

	LApplication();

	static LApplication* Instance();;

	virtual ~LApplication()
	{

	}

	virtual void Run();

	virtual void Init()
	{

	}

	virtual void MainLoop();
};
}
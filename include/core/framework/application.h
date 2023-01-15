

#include "core/foundation/config.h"

#include "core/framework/module.h"
#include "core/framework/luna_core.h"
#include "core/reflection/reflection.h"

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
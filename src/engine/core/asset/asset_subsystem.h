#pragma once

#include "core/subsystem/sub_system.h"
#include "core/file/path.h"
#include "core/memory/ptr.h"

namespace luna
{

template<typename T>
class AssetAsyncHandle
{

};

class AssetSubsystem : public SubSystem
{

public:
	bool OnPreInit() override;
	bool OnPostInit() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick() override;
	
public:
	template<typename T>
	LSharedPtr<T> LoadAsset(const LPath &path);

	template<typename T>
	AssetAsyncHandle<T> LoadAssetAsync(const LPath &path);


private:

};
}
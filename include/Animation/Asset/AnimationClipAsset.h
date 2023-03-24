#pragma once
#include "Core/Foundation/Container.h"
#include "Core/Object/BaseObject.h"
#include "Core/Asset/BinaryAsset.h"
#include "Animation/AnimationConfig.h"

namespace luna::animation
{
class ANIMATION_API AnimationClipAsset : public Asset
{
	RegisterTypeEmbedd(AnimationClipAsset, Asset)
public:
	AnimationClipAsset()
	{
	}
	//Asset资源读入到内存时回调
	virtual void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file) override;
	//Asset资源写入到磁盘时回调	
	virtual void OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data) override;
private:
	bool mReady = false;
};
}
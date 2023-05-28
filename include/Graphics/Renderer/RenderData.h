#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"


namespace luna::render
{
struct RenderData
{
	virtual void PerSceneUpdate(RenderScene* renderScene);
	virtual void PerViewUpdate(RenderView* renderView);
	virtual ~RenderData() {};
};

class RenderDataContainer
{
public:
	template<typename T>
	T* RequireData()
	{
		auto t = GetData<T>();
		if (!t)
		{
			t = new T;
			mDatas.push_back(t);
		}
		return t;
	}

	template<typename T>
	T* GetData()
	{
		for (auto it : mDatas)
		{
			auto p = dynamic_cast<T*>(it);
			if (p)
				return p;
		}
		return nullptr;
	}

	LArray<RenderData*> mDatas;
};

}

#pragma once

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"
#include <typeinfo>
namespace luna::graphics
{
struct RENDER_API RenderData
{
	virtual void PerSceneUpdate(RenderScene* renderScene){};
	
	virtual void PerViewUpdate(RenderView* renderView){};

	virtual void PerObjectUpdate(RenderObject* renderObject) {};

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
			const type_info& nInfo = typeid(T);
			LSharedPtr<T> newData = MakeShared<T>();
			mDatas.insert({ nInfo.hash_code(),newData});
			t = newData.get();
		}
		return t;
	}

	template<typename T>
	T* GetData()
	{
		const type_info& nInfo = typeid(T);
		size_t hashCode = nInfo.hash_code();
		auto itor = mDatas.find(hashCode);
		if (itor == mDatas.end())
		{
			return nullptr;
		}
		return dynamic_cast<T*>(itor->second.get());
	}

	template<typename T>
	const T* GetReadOnlyData() const
	{
		const type_info& nInfo = typeid(T);
		size_t hashCode = nInfo.hash_code();
		auto itor = mDatas.find(hashCode);
		if (itor == mDatas.end())
		{
			return nullptr;
		}
		return dynamic_cast<T*>(itor->second.get());
	}
	LUnorderedMap<size_t, LSharedPtr<RenderData>> mDatas;
	LArray<LSharedPtr<RenderData>> mDatas;
};

}

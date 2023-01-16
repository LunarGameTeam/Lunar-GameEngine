#pragma once

#include <cstdint>
#include <memory>
#include <concepts>

#include "Core/Foundation/Log.h"
#include "Core/CoreTypes.h"


namespace luna
{


class LObject;
class SharedObject;
class LType;

template<typename T>
class TPPtr;

class CORE_API SharedPtr
{
public:
	SharedPtr() noexcept
	{

	}

	SharedPtr(const SharedPtr& rv) :
		mPtr(rv.mPtr)
	{

	};

	SharedPtr(const SharedPtr&& rv) noexcept :
		mPtr(std::move(rv.mPtr))
	{

	};

	SharedPtr& operator=(const SharedPtr& value)
	{
		mPtr = value.mPtr;
		return *this;
	};

	SharedObject* operator->() const { return mPtr.get(); }

	std::shared_ptr<SharedObject> mPtr;
};
class CORE_API PPtr
{
public:
	PPtr() noexcept
	{
		//SubPtr必须拥有Parent
		assert(0);
	}

	explicit PPtr(LObject* Parent);

	PPtr(const PPtr& rv) :
		mInstanceID(rv.mInstanceID)
	{
		
	};

	PPtr(const PPtr&& rv) noexcept :
		mInstanceID(rv.mInstanceID)
	{

	};

	PPtr& operator=(const PPtr& value)
	{
		if (value.mInstanceID)
			mInstanceID = value.mInstanceID;
		else
			mInstanceID = 0;
		return *this;
	};

	~PPtr();

public:

	//待定
	LObject* Get();

	PPtr& operator=(LObject *val);

	void SetPtr(LObject *val);

protected:
	size_t mInstanceID = 0;
	LObject *mParent = nullptr;

};


template<typename T>
class TPPtr : public PPtr
{
public:
	TPPtr()
	{
		assert(0);
	}

	explicit TPPtr(LObject *Parent) : PPtr(Parent)
	{
		
	}

	TPPtr(const TPPtr && val) noexcept : PPtr(std::move(val))
	{
		//static_assert(std::is_base_of<LObject, T>::value, "T should inherit from LObject");
	};

	TPPtr(const TPPtr &)
	{
		assert(0);
	}

	TPPtr& operator=(const TPPtr & val)
	{
		PPtr::operator=(val);
		return *this;
	}

	~TPPtr() = default;
	//待定
	T* Get() const
	{
		return static_cast<T*>(T::InstanceIDToObject(mInstanceID));
	};

	explicit operator bool() const noexcept
	{
		return mInstanceID != 0;
	}

	void operator=(T *val)
	{
		SetPtr(static_cast<LObject*>(val));
	}

	T *operator->() const throw() { return static_cast<T *>(T::InstanceIDToObject(mInstanceID)); }

};


template<typename T>
class TPPtrArray
{
private:
	LObject* mReferencer = nullptr;
	std::vector<TPPtr<T>> mArray;
public:

	inline std::vector<TPPtr<T>>::iterator begin()
	{
		return mArray.begin();
	}

	inline std::vector<TPPtr<T>>::iterator end()
	{
		return mArray.end();
	}

	inline std::vector<TPPtr<T>>::const_iterator begin() const
	{
		return mArray.begin();
	}

	inline std::vector<TPPtr<T>>::const_iterator end() const
	{
		return mArray.end();
	}

	TPPtrArray()
	{
		assert(0);
	}

	explicit TPPtrArray(LObject* referencer) :
		mReferencer(referencer)
	{

	}

	void operator=(const TPPtrArray<T>& rv)
	{
		mArray = rv.mArray;
	}

	inline void PushBack(T* val)
	{
		mArray.emplace_back(mReferencer).SetPtr(val);
	}

	void Erase(T* val)
	{
		for (auto it = mArray.begin(); it != mArray.end(); it++)
		{
			if (it->Get() == val)
			{
				mArray.erase(it);
				break;
			}
		}

	}

	inline size_t Size() const
	{
		return mArray.size();
	}

	T* operator[](size_t index) const
	{
		return mArray[index].Get();
	}

};

}
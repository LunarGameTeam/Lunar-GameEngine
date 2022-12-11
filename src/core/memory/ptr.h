#pragma once

#include <cstdint>
#include <memory>
#include <concepts>

#include "core/foundation/log.h"
#include "core/core_types.h"


namespace luna
{

#pragma region Legacy Smart Pointer 

template<typename T>
using LSharedPtr = std::shared_ptr<T>;

template<typename T>
using LWeakPtr = std::weak_ptr<T>;

template<typename T>
using LUniquePtr = std::unique_ptr<T>;

template<typename T, typename... Args>
inline LSharedPtr<T> MakeShared(Args && ... args)
{
	return std::make_shared<T>(args...);
}

#pragma endregion

class LObject;
class LType;

extern LObject** s_NullPtr;

template<typename T>
class TSubPtr;

class CORE_API LSubPtr
{
public:
	LSubPtr() noexcept
	{
		//SubPtr必须拥有Parent
		assert(0);
	}

	LSubPtr(const LSubPtr &rv)
	{
		assert(0);
	};

	LSubPtr(const LSubPtr &&rv) noexcept
		:mPPtr(rv.mPPtr),
		mParent(rv.mParent)
	{

	};

	explicit LSubPtr(LObject *Parent);

	~LSubPtr();

public:
	void operator=(const LSubPtr & value) 
	{
		if (value.mPPtr)
			*mPPtr = *value.mPPtr;
		else
			*mPPtr = nullptr;
	};

	//待定
	LObject* Get()
	{
		return *mPPtr;
	}

	void operator=(LObject *val);
	void SetPtr(LObject *val);

protected:
	LObject **mPPtr = nullptr;
	LObject *mParent = nullptr;

};

template<typename T>
class TSubPtr : public LSubPtr
{
public:
	TSubPtr()
	{
		assert(0);
	}
	explicit TSubPtr(LObject *Parent) : LSubPtr(Parent)
	{
		
	}
	TSubPtr(const TSubPtr && val) noexcept : LSubPtr(std::move(val))
	{
		//static_assert(std::is_base_of<LObject, T>::value, "T should inherit from LObject");
	};
	TSubPtr(const TSubPtr &)
	{
		assert(0);
	}
	void operator=(const TSubPtr & val)
	{
		SetPtr(*val.mPPtr);
	}

	~TSubPtr() = default;
	//待定
	T* Get() const
	{
		return static_cast<T*>(*mPPtr);
	};

	explicit operator bool() const noexcept
	{
		return *mPPtr != nullptr;
	}

	void operator=(T *val)
	{
		SetPtr(static_cast<LObject*>(val));
	}

	T *operator->() const throw() { return static_cast<T *>(*mPPtr); }

};

struct CORE_API WeakPtrHandle
{
	WeakPtrHandle(LObject* _ptr) noexcept :
		ptr(_ptr)
	{

	}
	size_t weak_ref = 0;
	LObject *ptr = nullptr;
	void Inc() noexcept
	{
		++weak_ref;
	}
	void Dec() noexcept
	{
		--weak_ref;
		if (weak_ref == 0)
			delete this;
	}
};

template<typename T>
class TWeakPtr
{
public:
	TWeakPtr(T *ptr) noexcept
	{
		static_assert(std::is_base_of<LObject, T>::value, "T should inherit from LObject");

		if(ptr)
			mWeakHandle = ptr->GetHandle();

		if(mWeakHandle)
			mWeakHandle->Inc();
	}

	TWeakPtr(TWeakPtr&& rv) noexcept : mWeakHandle(rv.mWeakHandle) {};

	TWeakPtr(TWeakPtr &rv) noexcept : mWeakHandle(rv.mWeakHandle) 
	{
		if(mWeakHandle)
			mWeakHandle->Inc();
	}

	TWeakPtr() noexcept = default;

	void operator=(T *val) noexcept
	{
		if (val && mWeakHandle == val->GetHandle())
			return;

		if (mWeakHandle)
			mWeakHandle->Dec();
		
		if (val)
		{
			mWeakHandle = val->GetHandle();
			mWeakHandle->Inc();
		}
		else
		{
			mWeakHandle = nullptr;
		}			
	}

	T* operator*() const noexcept { return mWeakHandle ? mWeakHandle->ptr : nullptr; }

	void operator=(const TWeakPtr& val) noexcept
	{
		if (mWeakHandle == val.mWeakHandle)
			return;

		if (mWeakHandle)
			mWeakHandle->Dec();

		mWeakHandle = val.mWeakHandle;

		if (mWeakHandle)
			mWeakHandle->Inc();
	}

	T *get() noexcept { return mWeakHandle ? mWeakHandle->ptr : nullptr; }

	bool Valid() noexcept { return mWeakHandle->ptr != nullptr ? mWeakHandle : false;	}

	~TWeakPtr() noexcept
	{
		if (mWeakHandle)
			mWeakHandle->Dec();
	}

	WeakPtrHandle* mWeakHandle = nullptr;
};

template<typename T>
class TSubPtrArray
{
private:
	luna::LObject* m_parent = nullptr;
	std::vector<TSubPtr<T>> m_container;
public:

	inline std::vector<TSubPtr<T>>::iterator begin()
	{
		return m_container.begin();
	}

	inline std::vector<TSubPtr<T>>::iterator end()
	{
		return m_container.end();
	}

	inline std::vector<TSubPtr<T>>::const_iterator begin() const
	{
		return m_container.begin();
	}

	inline std::vector<TSubPtr<T>>::const_iterator end() const
	{
		return m_container.end();
	}

	TSubPtrArray()
	{
		assert(0);
	}

	explicit TSubPtrArray(LObject* parent) :
		m_parent(parent)
	{

	}

	void operator=(const TSubPtrArray<T>& rv)
	{
		m_container = rv.m_container;
	}

	void PushBack(T* val)
	{
		m_container.emplace_back(m_parent).SetPtr(val);
	}

	void Erase(T* val)
	{
		for (auto it = m_container.begin(); it != m_container.end(); it++)
		{
			if (it->Get() == val)
			{
				m_container.erase(it);
				break;
			}
		}

	}

	size_t Size() const
	{
		return m_container.size();
	}

	T* operator[](size_t index) const
	{
		return m_container[index].Get();
	}

};

}
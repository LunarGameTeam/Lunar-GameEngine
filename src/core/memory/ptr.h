#pragma once

#include <cstdint>
#include <memory>
#include <concepts>

#include "core/foundation/log.h"
#include "core/core_types.h"


namespace luna
{


class LObject;
class LType;

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
	LSubPtr& operator=(const LSubPtr & value)
	{
		if (value.mPPtr)
			*mPPtr = *value.mPPtr;
		else
			*mPPtr = nullptr;
		return *this;
	};

	//待定
	LObject* Get()
	{
		return *mPPtr;
	}

	LSubPtr& operator=(LObject *val);

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

template<typename T>
class TPPtr
{
public:
	TPPtr(T *ptr) noexcept
	{
		static_assert(std::is_base_of<LObject, T>::value, "T should inherit from LObject");
	}

	TPPtr(TPPtr&& rv) noexcept : mInstanceID(rv.mInstanceID) {};

	TPPtr(TPPtr&rv) noexcept : mInstanceID(rv.mInstanceID)
	{
	}

	TPPtr() noexcept = default;

	void operator=(T *val) noexcept
	{
		if (val && mInstanceID == val->GetInstanceID())
			return;

		
		if (val)
		{
			mInstanceID = val->GetInstanceID();
		}
		else
		{
			mInstanceID = 0;
		}			
	}

	T* operator*() const noexcept { return T::InstanceIDToObject(mInstanceID); }

	void operator=(const TPPtr& val) noexcept
	{
		if (mInstanceID == val.mInstanceID)
			return;

		mInstanceID = val.mInstanceID;

	}

	T* get() noexcept { return  T::InstanceIDToObject(mInstanceID); }

	~TPPtr() noexcept
	{
		mInstanceID = 0;
	}

	size_t mInstanceID = 0;	
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
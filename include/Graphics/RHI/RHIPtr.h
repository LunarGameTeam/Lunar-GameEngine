#pragma once
#include "Graphics/RenderConfig.h"
#include "memory"


namespace luna::graphics
{
template <typename T>
class TRHIPtr;


class RENDER_API RHIObject : public	 std::enable_shared_from_this<RHIObject>
{
public:
	RHIObject() = default;

	template <typename T>
	inline T* As() const
	{
		return static_cast<T*>(const_cast<RHIObject*>(this));
	}

	virtual ~RHIObject()
	{

	};
private:
};


class RHIPtr
{
public:
	RHIPtr() = default;

	RHIPtr(RHIPtr&& rval) noexcept :
		ptr(std::move(rval.ptr))
	{
	}

	RHIPtr(const RHIPtr& rval) noexcept :
		ptr(rval.ptr)
	{
	}


	explicit operator bool() const noexcept { return ptr.get() != nullptr; }

	inline RHIObject* get() const noexcept { return ptr.get(); }

protected:
	std::shared_ptr<RHIObject> ptr;

	template <typename T>
	friend class TRHIPtr;

	template <typename T, typename... Args>
	friend TRHIPtr<T> CreateRHIObject(Args&&... args);
};


template <typename T>
class TRHIPtr : public RHIPtr
{
public:
	TRHIPtr() = default;

	TRHIPtr(T* _p) noexcept
	{
		if(_p)
			ptr = _p->shared_from_this();
	}

	TRHIPtr(const TRHIPtr& lval) noexcept
	{
		ptr = lval.ptr;
	}

	TRHIPtr(const RHIPtr& lval)
	{
		if (lval)
		{
			T* _p = dynamic_cast<T*>(lval.get());
			ptr = lval.ptr;
		}
	}

	template <typename T2>
	TRHIPtr(TRHIPtr<T2>&& rval) noexcept
	{
		if (rval)
		{
			T* _p = dynamic_cast<T*>(rval.get());
			ptr.swap(rval.ptr);
		}
	}

	TRHIPtr(RHIPtr&& rval) noexcept
	{
		if (rval)
		{
			T* _p = dynamic_cast<T*>(rval.get());
			ptr.swap(rval.ptr);
		}
	}

	TRHIPtr& operator=(const TRHIPtr& _Right) noexcept
	{
		ptr = _Right.ptr;
		return *this;
	}

	TRHIPtr& operator=(TRHIPtr&& _Right) noexcept
	{
		ptr.swap(_Right.ptr);
		return *this;
	}

	operator T*() const noexcept
	{
		return static_cast<T*>(ptr.get());
	}

	operator bool () noexcept
	{
		return ptr.get() != nullptr;
	}


	T* get() { return dynamic_cast<T*>(ptr.get()); }

	const T* get()const { return dynamic_cast<const T*>(ptr.get()); }

	[[nodiscard]] T* operator->() const noexcept { return dynamic_cast<T*>(ptr.get()); }

protected:
	friend class RHIPtr;

	template <typename T, typename... Args>
	friend TRHIPtr<T> MakeShared(Args&&... args);
};


template <typename T, typename... Args>
TRHIPtr<T> CreateRHIObject(Args&&... args)
{
	TRHIPtr<T> res;
	T* t = new T(args...);
	res.ptr.reset((RHIObject*)t);
	return res;
}
}

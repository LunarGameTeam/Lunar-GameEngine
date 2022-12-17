#pragma once

#include <functional>

#include <list>

#include <memory>

namespace luna
{

struct ActionBase
{

};

template<typename RetVal, typename... Param>
struct SignalActionList;


template<typename RetVal, typename... Param>
struct SignalAction : ActionBase
{
	using std_function_t = std::function<RetVal(Param...)>;
	using action_list_t = SignalActionList<RetVal, Param...>;

	std_function_t mRealFunc;
	std::weak_ptr<action_list_t> mOwner;


	~SignalAction()
	{
		auto ptr = mOwner.lock();
		if (!ptr)
			return;		
		ptr->mActions.remove_if([this](auto ref) {
			return ref.lock().get() == this;
		});
		
	}
};

template<typename RetVal, typename... Param>
struct SignalActionList
{
	using action_t = SignalAction<RetVal, Param...>;

	void Broadcast(Param&& ...params)
	{
		for (auto it : mActions)
		{
			if (!it.expired())
			{
				it.lock()->mRealFunc(std::forward<Param>(params)...);
			}
		}

	}
public:
	std::list<std::weak_ptr<action_t>> mActions;
};

using ActionHandle = std::shared_ptr< ActionBase>;


template<typename RetVal, typename... Param>
struct Signal
{	
	using action_t = SignalAction<RetVal, Param...>;
	using action_list_t = SignalActionList<RetVal, Param...>;
public:
	Signal()
	{
		mListeners.reset(new action_list_t());
	}

	ActionHandle Bind(action_t::std_function_t func)
	{
		action_t* action = new action_t();
		action->mRealFunc = func;
		action->mOwner = mListeners;

		ActionHandle actionHandle((ActionBase*)action);
		std::shared_ptr<action_t> actionHandle2 = std::static_pointer_cast<action_t>(actionHandle);
		std::weak_ptr<action_t> weakPtr = actionHandle2;
		mListeners->mActions.emplace_back(weakPtr);
		return actionHandle;
	}

	inline void BroadCast(Param&&... param) const
	{
		mListeners->Broadcast(std::forward<Param>(param)...);
	}

	void Clear()
	{
		mListeners->mActions.clear();
	}

private:
	std::shared_ptr<action_list_t> mListeners;
};

#define SIGNAL(name,...) Signal<void, __VA_ARGS__> name;

}
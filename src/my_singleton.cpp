#include "chuck_vm_manager.hpp"
#include "my_singleton.hpp"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

MySingleton *MySingleton::singleton = nullptr;

void MySingleton::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("hello_singleton", "label"), &MySingleton::hello_singleton);

    ADD_SIGNAL(MethodInfo("get_global_callbacks_processed"));
}

MySingleton *MySingleton::get_singleton()
{
	return singleton;
}

MySingleton::MySingleton()
{
	ERR_FAIL_COND(singleton != nullptr);
	singleton = this;
}

MySingleton::~MySingleton()
{
	ERR_FAIL_COND(singleton != this);
	singleton = nullptr;
}

void MySingleton::hello_singleton(godot::Label *label)
{
	label->set_text(label->get_text() + "Hello GDExtension Singleton!\n");
}

t_CKINT MySingleton::get_next_callback_id()
{
	t_CKINT nextID = m_nextCallbackID;

	++m_nextCallbackID;
	if (m_nextCallbackID == CK_INT_MAX)
	{
		m_nextCallbackID = 0;
	}

	return nextID;
}

void MySingleton::process_global_variable_callback_results()
{
	std::vector<CallbackResult> callbackResultsToProcess;
	callbackResultsToProcess.reserve(m_callbackResults.size());

	{
		// Don't know the thread situation yet but it's possible Chuck is making callbacks at this point.
		// Don't do too much processing during the lock, just copy results to a temporary vector for processing.
		std::lock_guard<std::mutex> lock(m_callbackResultMutex);
		while (!m_callbackResults.empty())
		{
			callbackResultsToProcess.push_back(m_callbackResults.front());
			m_callbackResults.pop();
		}
	}

    for (auto callbackResult : callbackResultsToProcess)
    {
		// Callback results don't give us the variable name or info about which VM called it.
		// So we have to re-associate the result to the original context using the ID.
        for (size_t i = 0; i < m_callbackContexts.size(); ++i)
        {
			auto& callbackContext = m_callbackContexts[i];
            if (callbackResult.m_callbackID == callbackContext.m_callbackID)
            {
				auto variable = callbackContext.m_vmManager->find_registered_global_variable(callbackContext.m_variableName);
                if (variable != nullptr)
				{
                    variable->value = callbackResult.m_variableValue;
				}
				
				m_callbackContexts.erase(m_callbackContexts.begin() + i);
				break;
            }
        }
    }
	
	emit_signal("get_global_callbacks_processed");
}

void MySingleton::get_global_int_callback(t_CKINT callbackID, t_CKINT globalValue)
{
    CallbackResult newCallbackResult;
    newCallbackResult.m_callbackID = callbackID;
    newCallbackResult.m_variableValue = globalValue;

	// Quickly collect results to be processed later, all in one batch.
	std::lock_guard<std::mutex> lock(get_singleton()->m_callbackResultMutex);
    get_singleton()->m_callbackResults.push(newCallbackResult);
}

void MySingleton::get_global_float_callback(t_CKINT callbackID, t_CKFLOAT globalValue)
{
    CallbackResult newCallbackResult;
    newCallbackResult.m_callbackID = callbackID;
    newCallbackResult.m_variableValue = globalValue;

	// Quickly collect results to be processed later, all in one batch.
	std::lock_guard<std::mutex> lock(get_singleton()->m_callbackResultMutex);
    get_singleton()->m_callbackResults.push(newCallbackResult);
}

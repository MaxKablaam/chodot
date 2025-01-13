#pragma once

#include <any>
#include <chuck.h>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/label.hpp>

using namespace godot;

class ChuckVMManager;

struct CallbackContext
{
	t_CKINT m_callbackID;
	ChuckVMManager* m_vmManager;
	std::string m_variableName;
};

struct CallbackResult
{
	t_CKINT m_callbackID;
	std::any m_variableValue;
};


class MySingleton : public Object
{
	GDCLASS(MySingleton, Object);

	static MySingleton *singleton;

protected:
	static void _bind_methods();

	t_CKINT get_next_callback_id();

	t_CKINT m_nextCallbackID = 0;
	std::vector<CallbackContext> m_callbackContexts;
	std::mutex m_callbackResultMutex;
	std::queue<CallbackResult> m_callbackResults;

public:
	static MySingleton *get_singleton();

	MySingleton();
	~MySingleton();

	void hello_singleton(godot::Label *label);

	void add_new_callback_context(CallbackContext& callbackContext);
	void process_global_variable_callback_results();

	static void get_global_int_callback(t_CKINT callbackID, t_CKINT globalValue);
	static void get_global_float_callback(t_CKINT callbackID, t_CKFLOAT globalValue);
};

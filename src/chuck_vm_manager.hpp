#pragma once

#include <any>
#include <unordered_map>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <chuck.h>
#include <chuck_globals.h>

using namespace godot;

class ChuckVMManager : public Node
{
	GDCLASS(ChuckVMManager, Node);

public:
	struct GlobalVariableContainer
	{
		enum class Type
		{
			Int,
			Float
		};
		Type type;
		std::string name;
		std::any value;
	};

private:
	// our ChucK instance
	ChucK * the_chuck = NULL;
	// audio input buffer
	SAMPLE * g_inputBuffer = NULL;
	// audio output buffer
	SAMPLE * g_outputBuffer = NULL;
	// our audio buffer size
	t_CKINT g_bufferSize = 0;

	// Reference to godot audio stream player
	AudioStreamPlayer* audio_stream_player = nullptr;

	static std::unordered_map<std::string, ChuckVMManager*> instance_map;
	std::vector<std::string> registered_events;

	std::vector<GlobalVariableContainer> registered_global_variables;

	// shred id
    t_CKUINT shredID = 0;
    // our shred ID stack
    vector<t_CKUINT> shredIDs;

	// global event array
	vector<string> global_events;

	static void all_globals_cb_static(const vector<Chuck_Globals_TypeValue> &list, void *data);
	void all_globals_cb( const vector<Chuck_Globals_TypeValue> & list);

	// TODO: event_broadcast()
	// It should listen for all events, and then send a signal
	// Signal should have name of the event, and args if possible
	static void event_listener_cb_static(const char* name);
	void event_listener_cb(const char* name);

	// allocate global audio buffers
	void alloc_global_buffers( t_CKINT bufferSize );
	// de-allocate global audio buffers
	void cleanup_global_buffers();

protected:
	static void _bind_methods();

public:
	ChuckVMManager();
	~ChuckVMManager();

	void _ready() override;
	void _process(double delta) override;

	godot::String hello_node();

	// Shred Manipulation
	void run_code(godot::String code);
	void add_shred(godot::String content);
	void remove_last_shred();
	void remove_shred(int _shredID);
	void remove_all_shreds();
	godot::PackedInt32Array get_shred_ids();

	// Globals
	void register_global_events();
	void broadcast_global_event(String name);
	//TODO: signal_globael_event
	void set_global_float(const String& name, double value);
	void set_global_int(const String& name, int value);
	void register_global_float(const String& name);
	void register_global_int(const String& name);
	GlobalVariableContainer* find_registered_global_variable(const std::string& name);
	t_CKFLOAT get_global_float(const String& name);
	int64_t get_global_int(const String& name);

	// Setters and Getters
	void set_audio_stream_player(AudioStreamPlayer* p_audio_stream_player) { audio_stream_player = p_audio_stream_player; };
	AudioStreamPlayer* get_audio_stream_player() const { return audio_stream_player; };
};

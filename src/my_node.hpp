#pragma once

#include <godot_cpp/classes/node.hpp>
#include <chuck.h>

using namespace godot;

class MyNode : public Node //TODO: real name should be something like ChuckVM
{
	GDCLASS(MyNode, Node);
private:
	// our ChucK instance
	ChucK * the_chuck = NULL;
	// audio input buffer
	SAMPLE * g_inputBuffer = NULL;
	// audio output buffer
	SAMPLE * g_outputBuffer = NULL;
	// our audio buffer size
	t_CKINT g_bufferSize = 0;

	// allocate global audio buffers
	void alloc_global_buffers( t_CKINT bufferSize );
	// de-allocate global audio buffers
	void cleanup_global_buffers();

protected:
	static void _bind_methods();

public:
	MyNode();
	~MyNode();

	void _ready() override;
	void _process(double delta) override;

	godot::String hello_node();

	int main(int argc, char ** argv);
	void init_chuck();
	void ready_chuck();
	int process_chuck();
	void cleanup_chuck();
};

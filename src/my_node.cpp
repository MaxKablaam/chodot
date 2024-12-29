#include "my_node.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/audio_stream_generator_playback.hpp>
#include <godot_cpp/variant/utility_functions.hpp>


using namespace godot;

static void _print(const char *s)
{
	UtilityFunctions::print(s);
}

void MyNode::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("hello_node"), &MyNode::hello_node);
}

MyNode::MyNode()
{
	init_chuck();
}

MyNode::~MyNode()
{
	cleanup_chuck();
}

// Override built-in methods with your own logic. Make sure to declare them in the header as well!

void MyNode::_ready()
{
	ready_chuck();
}

void MyNode::_process(double delta)
{
	if(process_chuck() == 1)
	{
		// do something
	}
}

godot::String MyNode::hello_node()
{
	return "Hello GDExtension Node\n";
}

void MyNode::init_chuck()
{
	// ---- Init Chuck ----

	// instantiate a ChucK instance
    the_chuck = new ChucK();
	// set some parameters: sample rate
    the_chuck->setParam( CHUCK_PARAM_SAMPLE_RATE, 44100 );
    // number of input channels
    the_chuck->setParam( CHUCK_PARAM_INPUT_CHANNELS, 2 );
    // number of output channels
    the_chuck->setParam( CHUCK_PARAM_OUTPUT_CHANNELS, 2 );
    // whether to halt the VM when there is no more shred running
    the_chuck->setParam( CHUCK_PARAM_VM_HALT, TRUE );
    // set hint so internally can advise things like async data writes etc.
    the_chuck->setParam( CHUCK_PARAM_IS_REALTIME_AUDIO_HINT, FALSE );
    // turn on logging to see what ChucK is up to; higher == more info
    // chuck->setLogLevel( 3 );

	// allocate audio buffers, also after parameters are set
    alloc_global_buffers( 1024 );

	// initialize ChucK, after the parameters are set
    the_chuck->init();
}

void MyNode::ready_chuck()
{
	// start ChucK VM and synthesis engine
    the_chuck->start();

	// compile some ChucK code from string (this can be called on-the-fly and repeatedly)
    if( !the_chuck->compileCode( "repeat(10) { <<< \"hello ChucK! random integer:\", Math.random2(1,100) >>>; }", "", 1 ) )
    {
        // got error, baillng out...
        exit( 1 );
    }
}

int MyNode::process_chuck()
{
	// keep going as long as a shred is still running
    // (as long as CHUCK_PARAM_VM_HALT is set to TRUE)
    if( the_chuck->vm_running() )
    {
        // run ChucK for the next `bufferSize`
        // pre-condition: `input` constains any audio input
        // post-condition: `output` constains any synthesized audio
        the_chuck->run( g_inputBuffer, g_outputBuffer, g_bufferSize );
		return 1;
    }
	else
	{
		return 0;
	}
}

void MyNode::cleanup_chuck()
{
	// deallocate
    cleanup_global_buffers();
    
    // clean up ChucK
    CK_SAFE_DELETE( the_chuck );
}

void MyNode::alloc_global_buffers( t_CKINT bufferSize )
{
    // good practice to clean, in case this function is called more than once
    cleanup_global_buffers();

    // allocate audio buffers as expected by ChucK's run() function below
    g_inputBuffer = new SAMPLE[bufferSize * the_chuck->getParamInt(CHUCK_PARAM_INPUT_CHANNELS)];
    g_outputBuffer = new SAMPLE[bufferSize * the_chuck->getParamInt(CHUCK_PARAM_OUTPUT_CHANNELS)];
    // zero out
    memset( g_inputBuffer, 0, bufferSize * the_chuck->getParamInt(CHUCK_PARAM_INPUT_CHANNELS) );
    memset( g_outputBuffer, 0, bufferSize * the_chuck->getParamInt(CHUCK_PARAM_OUTPUT_CHANNELS) );
    // set the buffer size
    g_bufferSize = bufferSize;
}

void MyNode::cleanup_global_buffers()
{
    // reclaim memory
    CK_SAFE_DELETE_ARRAY( g_inputBuffer );
    CK_SAFE_DELETE_ARRAY( g_outputBuffer );
    // reset
    g_bufferSize = 0;
}

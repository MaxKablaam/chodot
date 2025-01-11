class_name ChuckVM extends ChuckVMManager

func _ready() -> void:
	chuck_event.connect(_on_chuck_event)
	#run_code("repeat(10) { <<< \"hello ChucK! random integer:\", Math.random2(1,100) >>>; }")
	run_code("global Event BigEvent;
			global Event the_nextNote;
			global float the_frequency;
			global float filter_freq;

			// patch
			Blit s => LPF filter => ADSR e => JCRev r => dac;
			.5 => s.gain;
			.05 => r.mix;
			2000.0 => filter_freq => filter.freq;
			// set adsr
			e.set( 5::ms, 3::ms, .5, 5::ms );

			// an array
			[ 0, 2, 4, 7, 9, 11 ] @=> int hi[];

			// infinite time loop
			while( true )
			{
				// frequency
				Std.mtof( 33 + Math.random2(0,3) * 12 +
					hi[Math.random2(0,hi.size()-1)] ) => s.freq;
				// copy to global
				s.freq() => the_frequency;
				filter_freq => filter.freq;

				// harmonics
				Math.random2( 1, 5 ) => s.harmonics;

				// key on
				e.keyOn();

				BigEvent.broadcast();

				// wait on event
				the_nextNote => now;
				// key off
				e.keyOff();
				// advance time for release
				5::ms => now;
			}
			")
	#add_shred(path)
	await get_tree().create_timer(1).timeout
	for i in range(10):
		set_global_float("filter_freq", randf_range(200, 500))
		broadcast_global_event("the_nextNote")
		await get_tree().create_timer(0.5).timeout
	#remove_all_shreds()


func _on_chuck_event(name: String):
	print("Got the event signal! -- ", name)

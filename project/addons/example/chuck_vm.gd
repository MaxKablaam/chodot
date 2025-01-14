class_name ChuckVM extends ChuckVMManager

@export var chuck_file: ChuckFile

var global_name_frequency = "the_frequency"
var global_name_filter = "filter_freq"

func _ready() -> void:
	
	# This will setup asynchronous "get global" calls to ChucK every frame.
	# From here on, you can call get_global_float from GDScript to
	# synchronously get the value from the latest callback.
	watch_global_float(global_name_frequency)
	watch_global_float(global_name_filter)
	
	# Gives us a callback each frame where all global variables are the most up-to-date.
	MySingleton.connect("get_global_callbacks_processed", _on_global_variables_processed)
	
	chuck_event.connect(_on_chuck_event)
	run_code(chuck_file.contents)
	await get_tree().create_timer(1).timeout
	for i in range(10):
		set_global_float("filter_freq", randf_range(200, 500))
		broadcast_global_event("the_nextNote")
		await get_tree().create_timer(0.5).timeout
	remove_all_shreds()

# This will be called every frame, after global variables have been updated from ChucK callbacks.
func _on_global_variables_processed():
	# Very spammy, uncomment to test out global variable reads.
	#print(global_name_frequency, "=", get_global_float(global_name_frequency))
	#print(global_name_filter, "=", get_global_float(global_name_filter))
	pass

func _on_chuck_event(name: String):
	print("Got the event signal! -- ", name)

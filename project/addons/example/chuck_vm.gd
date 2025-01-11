class_name ChuckVM extends ChuckVMManager

@export var chuck_file: ChuckFile

func _ready() -> void:
	chuck_event.connect(_on_chuck_event)
	#run_code("repeat(10) { <<< \"hello ChucK! random integer:\", Math.random2(1,100) >>>; }")
	run_code(chuck_file.contents)
	#add_shred(path)
	await get_tree().create_timer(1).timeout
	for i in range(10):
		set_global_float("filter_freq", randf_range(200, 500))
		broadcast_global_event("the_nextNote")
		await get_tree().create_timer(0.5).timeout
	#remove_all_shreds()


func _on_chuck_event(name: String):
	print("Got the event signal! -- ", name)

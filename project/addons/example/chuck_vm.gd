class_name ChuckVM extends MyNode

@export var chuck_files: Array[ChuckFile]

func _ready() -> void:
	for file in chuck_files:
		var path := ProjectSettings.globalize_path(file.resource_path)
		add_shred(path)
	await get_tree().create_timer(2).timeout
	print_all_globals()
	await get_tree().create_timer(2).timeout
	var ids = get_shred_ids()
	remove_all_shreds()

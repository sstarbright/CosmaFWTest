class_name LevelData
extends RefCounted

var map_size := Vector2i(3,3)
var map_data : Array[Array]

var texture_slots : Array[TextureSlotData]

var ambient_color : Color

var fog_color : Color
var fog_start := 0.0
var fog_end := 8.0

var corner_color : Color
var corner_start := 2.0
var corner_end := 0.1

var floor_texture : TextureSlotData
var ceiling_texture : TextureSlotData
var skybox_texture : TextureSlotData

var spawn_point := Vector2(1,1)
var spawn_viewpoint : float = 0.0

func load_level_file(path : String):
	pass

func save_level_file(path : String):
	pass

func _init(level_editor : LevelEditor, path := "") -> void:
	if path.length() > 0:
		load_level_file(path)
	else:
		pass

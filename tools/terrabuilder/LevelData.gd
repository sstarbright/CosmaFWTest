class_name LevelData
extends RefCounted

var map_size := Vector2i(3,3)
var starting_pos := Vector2i(1,1)
var map_data : Array[Array]

var texture_slots : Array[TextureSlotData]

var floor_color : Color
var ceiling_color : Color

var skybox_texture : TextureSlotData

var fog_color : Color
var fog_start := 0.0
var fog_end := 8.0

var corner_color : Color
var corner_sharpness := 2.0
var corner_weakness := 0.1

func load_level_file(path : String):
	pass

func save_level_file(path : String):
	pass

func _init(path := "") -> void:
	if path.length() > 0:
		load_level_file(path)

class_name TileProperties
extends Resource

enum TileRenderMode {
	None,
	Wall,
	Floor,
	Ceiling
};

var has_collision := false
var render_mode : TileRenderMode = TileRenderMode.None

var mirror_u := false
var mirror_v := false
var swap_uv := false

var pad_north := false
var pad_south := false
var pad_east := false
var pad_west := false

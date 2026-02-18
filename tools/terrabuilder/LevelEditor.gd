class_name LevelEditor
extends Control

@onready var tool_bar_file : PopupMenu = $PanelContainer/VBoxContainer/Header/MenuButton.get_popup()
@onready var loaded_file_label : Label = $PanelContainer/VBoxContainer/Header/Label
@onready var new_file_dialog : FileDialog = $NewMapFile
@onready var open_file_dialog : FileDialog = $OpenMapFile
@onready var save_as_file_dialog : FileDialog = $SaveAsMapFile
@onready var not_saved_warning : AcceptDialog = $NotSavedWarning

signal finished_not_saved

func _ready() -> void:
	LevelEditorData.loaded_map_data = LevelData.new(self, "")
	tool_bar_file.id_pressed.connect(file_menu_selected)
	get_tree().set_auto_accept_quit(false)
	get_window().close_requested.connect(main_window_closed)

func _process(delta: float) -> void:
	if Input.is_action_just_pressed("SaveMap"):
		save_map_file()

func shut_down_editor():
	get_tree().root.propagate_notification(NOTIFICATION_WM_CLOSE_REQUEST)
	get_tree().quit()

func main_window_closed():
	LevelEditorData.is_quitting = true
	var should_close = await checK_not_saved()
	if should_close:
		shut_down_editor()

func file_menu_selected(id : int):
	match id:
		0:
			var should_open = await checK_not_saved()
			if should_open:
				new_file_dialog.show()
		1:
			var should_open = await checK_not_saved()
			if should_open:
				open_file_dialog.show()
		2:
			save_map_file()
		3:
			if LevelEditorData.loaded_map_path.length() > 0:
				save_as_file_dialog.show()
		_:
			pass

func new_file_selected(path: String):
	LevelEditorData.loaded_map_path = path
	loaded_file_label.text = "Loaded \"" + path + "\""

func existing_file_selected(path: String):
	LevelEditorData.loaded_map_path = path
	loaded_file_label.text = "Loaded \"" + path + "\""

func save_as_file_selected(path: String) -> void:
	LevelEditorData.loaded_map_path = path
	LevelEditorData.is_saved = false
	save_map_file()

func discard_file():
	if LevelEditorData.is_quitting:
		shut_down_editor()
	else:
		LevelEditorData.discard_file = true
		finished_not_saved.emit()

func keep_file():
	LevelEditorData.discard_file = false
	LevelEditorData.is_quitting = false
	finished_not_saved.emit()

func save_map_file():
	if !LevelEditorData.is_saved && LevelEditorData.loaded_map_path.length() > 0:
		print("SAVED MAP")
		LevelEditorData.is_saved = true

func checK_not_saved() -> bool:
	if !LevelEditorData.is_saved:
		not_saved_warning.show()
		await finished_not_saved
		not_saved_warning.hide()
		if !LevelEditorData.discard_file:
			return false
	return true

func changes_made():
	LevelEditorData.is_saved = false

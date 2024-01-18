return {
	disable_netrw = false,
	hijack_netrw = false,
	hijack_cursor = true,
	update_focused_file = {
		enable = true,
		update_cwd = true,
		ignore_list = {}
	},
	diagnostics = {
		enable = true,
		icons = {
			hint = "!",
			info = "i",
			warning = "!!",
			error = "x"
		}
	},
	view = {
		width = 30,
		side = 'left'
	},
	renderer = {
		root_folder_modifier = ":~",
		indent_markers = {
			enable = false,
			icons = {
				corner = "└ ",
				edge = "│ ",
				none = "  "
			}
		}
	}
}

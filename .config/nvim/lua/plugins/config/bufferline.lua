local status_ok, bufferline = pcall(require, "bufferline")
if not status_ok then
	return
end
local colors = require("colors")

bufferline.setup {
	highlights = {
		background = {
			fg = colors.white,
		},
		close_button = {
			fg = colors.white,
			bg = colors.black
		},
		buffer_selected = {
			bold = true,
			bg = colors.bg
		},
		close_button_selected = {
			bold = true,
			bg = colors.bg
		},
	},
	options = {
		offsets = { { filetype = "NvimTree", text = "" } },
		close_command = "bdelete! %d",       -- can be a string | function, see "Mouse actions"
		buffer_close_icon = "",
		modified_icon = "*",
		show_close_icon = false,
		left_trunc_marker = "<",
		right_trunc_marker = ">",
		show_tab_indicators = true,
		enforce_regular_tabs = true,
		view = "multiwindow",
		show_buffer_close_icons = true,
		always_show_bufferline = false,
		diagnostics = "nvim_lsp",
		diagnostics_indicator = function(_, _, diagnostics_dict)
			local s = " "
			for e, n in pairs(diagnostics_dict) do
				local sym = e == "error" and "X"
				or (e == "warning" and "!" or "i")
				s = s .. sym .. n
			end
			return s
		end
	}
}

vim.cmd([[
 	autocmd ColorScheme * highlight BufferLineFill guibg=none
]])

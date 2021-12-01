local present, bufferline = pcall(require, "bufferline")
if not present then
	return
end

local colors = require("colors")

bufferline.setup {
	options = {
		offsets = { { filetype = "NvimTree", text = "" } },
		buffer_close_icon = "",
		modified_icon = "*",
		show_close_icon = false,
		left_trunc_marker = "<",
		right_trunc_marker = ">",
		show_tab_indicators = true,
		enforce_regular_tabs = true,
		view = "multiwindow",
		show_buffer_close_icons = true,
		always_show_bufferline = true,
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
	},
	highlights = {
		fill = {
			guifg = colors.white,
			guibg = colors.bg
		}
	}
}

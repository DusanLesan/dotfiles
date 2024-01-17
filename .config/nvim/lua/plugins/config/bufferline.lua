vim.cmd([[
 	autocmd ColorScheme * highlight BufferLineFill guibg=none
]])

return {
	options = {
		offsets = { { filetype = "NvimTree", text = "" } },
		close_command = "bdelete! %d",
		buffer_close_icon = "x",
		modified_icon = "*",
		show_close_icon = false,
		left_trunc_marker = "<",
		right_trunc_marker = ">",
		show_tab_indicators = false,
		enforce_regular_tabs = false,
		view = "multiwindow",
		show_buffer_close_icons = true,
		always_show_bufferline = false,
		diagnostics = "nvim_lsp",
		separator_style = { '', '' },
		indicator = { style = 'none' },
		diagnostics_indicator = function(_, _, diagnostics_dict)
			local s = " "
			for e, n in pairs(diagnostics_dict) do
				local sym = e == "error" and "E"
				or (e == "warning" and "!" or "i")
				s = s .. sym .. n
			end
			return s
		end
	}
}

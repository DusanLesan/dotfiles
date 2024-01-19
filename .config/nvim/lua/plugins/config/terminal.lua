local M = 	{
	"akinsho/toggleterm.nvim",
	cmd = { "ToggleTerm" }
}

M.opts = {
	open_mapping = [[<a-t>]],
	on_config_done = nil,
	size = 12,
	hide_numbers = true,
	shade_filetypes = {},
	shade_terminals = false,
	start_in_insert = true,
	insert_mappings = true,
	direction = "horizontal",
	close_on_exit = true,
	float_opts = {
		border = "curved",
		winblend = 0,
		highlights = {
			border = "Normal",
			background = "Normal"
		}
	},
}

return M

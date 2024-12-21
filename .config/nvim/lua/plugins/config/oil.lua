local M = {
	"stevearc/oil.nvim",
	cmd = "Oil"
}

M.opts = {
	keymaps = {
		["<leader>e"] = ":lua require('oil').discard_all_changes()<CR>:lua require('oil').close()<CR>",
		["q"] = "actions.close",
		["<C-r>"] = "actions.refresh",
		["<A-Left>"] = "actions.parent",
		["<enter>"] = "actions.select",
		["g?"] = "actions.show_help",
		["<CR>"] = "actions.select",
		["<C-s>"] = "actions.select_vsplit",
		["<C-h>"] = "actions.select_split",
		["<C-t>"] = "actions.select_tab",
		["<C-p>"] = "actions.preview",
		["_"] = "actions.open_cwd",
		["`"] = "actions.cd",
		["~"] = "actions.tcd",
		["gs"] = "actions.change_sort",
		["gx"] = "actions.open_external",
		["g."] = "actions.toggle_hidden"
	},
	view_options = {
		show_hidden = true
	},
	win_options = {
		wrap = true
	}
}

return M

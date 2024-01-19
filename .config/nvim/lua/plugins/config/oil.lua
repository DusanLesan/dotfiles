local M = {
	"stevearc/oil.nvim",
	cmd = "Oil"
}

local opts = {
	keymaps = {
		["<leader>e"] = ":lua require('oil').discard_all_changes()<CR>:lua require('oil').close()<CR>",
		["q"] = "actions.close",
		["<C-r>"] = "actions.refresh",
		["<Left>"] = "actions.parent",
		["<Right>"] = "actions.select",
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
	float = {
		padding = 2,
		border = "rounded",
		win_options = {
			winblend = 1
		}
	},
	win_options = {
		wrap = true
	}
}

function M.config()
	require("oil").setup(opts)
	vim.keymap.set("n", "<leader>to", "<CMD>oil.open_float<CR>", { desc = "Open parent directory" })
end

return M

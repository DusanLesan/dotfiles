local M = {
	"lewis6991/gitsigns.nvim",
	event = "VeryLazy",
	ft = { "gitcommit", "diff" }
}

M.opts = {
	signs = {
		add = { text = '┃' },
		change = { text = '┃' },
		delete = { text = '_' },
		topdelete = { text = '‾' },
		changedelete = { text = '~' },
		untracked = { text = '┆' },
	},
	current_line_blame_opts = {
		virt_text_pos = 'right_align',
		delay = 750
	},
	signcolumn = false,
	numhl = true
}

return M

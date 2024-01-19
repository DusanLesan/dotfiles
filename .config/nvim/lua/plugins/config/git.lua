local M = 	{
	"lewis6991/gitsigns.nvim",
	event = "VeryLazy",
	ft = { "gitcommit", "diff" }
}

M.opts = {
	signs = {
		add = {hl = 'GitSignsAdd' , text = '│', numhl='GitSignsAddNr' , linehl='GitSignsAddLn'},
		change = {hl = 'GitSignsChange', text = '│', numhl='GitSignsChangeNr', linehl='GitSignsChangeLn'},
		delete = {hl = 'GitSignsDelete', text = '_', numhl='GitSignsDeleteNr', linehl='GitSignsDeleteLn'},
		topdelete = {hl = 'GitSignsDelete', text = '‾', numhl='GitSignsDeleteNr', linehl='GitSignsDeleteLn'},
		changedelete = {hl = 'GitSignsChange', text = '~', numhl='GitSignsChangeNr', linehl='GitSignsChangeLn'}
	},
	current_line_blame_opts = {
		virt_text_pos = 'right_align',
		delay = 750
	},
	signcolumn = false,
	numhl = true
}

return M

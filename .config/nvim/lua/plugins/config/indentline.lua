vim.g.indent_blankline_buftype_exclude = { "terminal", "nofile" }
vim.g.indent_blankline_filetype_exclude = {
	"help",
	"startify",
	"dashboard",
	"packer",
	"neogitstatus",
	"NvimTree",
	"Trouble"
}

vim.opt.list = true
vim.opt.listchars:append("space:⋅")
vim.g.indent_blankline_space_char_blankline = " "
vim.g.indentLine_enabled = 1
vim.g.indent_blankline_show_trailing_blankline_indent = false
vim.g.indent_blankline_use_treesitter = true
vim.g.indent_blankline_show_current_context = true
vim.g.indent_blankline_context_patterns = {
	"class",
	"return",
	"function",
	"method",
	"block",
	"arguments",
	"if_statement",
	"else_clause",
	"try_statement",
	"catch_clause",
	"import_statement",
	"operation_type",
	"^if",
	"^while",
	"^for",
	"^case",
	"^switch"
}

vim.opt.list = true
vim.opt.listchars:append("space:⋅")

return {
	enabled = true,
	scope = { enabled = true },
	indent = {
		char = "│",
		tab_char = "│",
		smart_indent_cap = false
	},
	exclude = {
		filetypes = {
			"help",
			"terminal",
			"lazy",
			"lspinfo",
			"TelescopePrompt",
			"TelescopeResults",
			"mason",
			"text",
			"toggleterm"
		},
		buftypes = {
			"terminal",
			"nofile"
		}
	}
}

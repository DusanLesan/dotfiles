local lazypath = vim.fn.stdpath("data") .. "/lazy/lazy.nvim"
if not vim.loop.fs_stat(lazypath) then
	vim.fn.system({
		"git",
		"clone",
		"--filter=blob:none",
		"https://github.com/folke/lazy.nvim.git",
		"--branch=stable",
		lazypath
	})
end
vim.opt.rtp:prepend(lazypath)

require("lazy").setup({
	{ import = "plugins.config" },

	"mg979/vim-visual-multi",

	{
		"folke/which-key.nvim",
		event = "VeryLazy"
	},

	{
		"max397574/better-escape.nvim",
		event = "InsertEnter",
		opts = {
			timeout = vim.o.timeoutlen
		}
	},

	{
		"numToStr/Comment.nvim",
		opts = {},
		keys = {
			{ "gcc", mode = "n", desc = "Comment toggle current line" },
			{ "gc", mode = { "n", "o" }, desc = "Comment toggle linewise" },
			{ "gc", mode = "x", desc = "Comment toggle linewise (visual)" },
			{ "gbc", mode = "n", desc = "Comment toggle current block" },
			{ "gb", mode = { "n", "o" }, desc = "Comment toggle blockwise" },
			{ "gb", mode = "x", desc = "Comment toggle blockwise (visual)" }
		}
	},

	{
		"windwp/nvim-autopairs",
		event = "InsertEnter",
		opts = {}
	},

	{
		"MeanderingProgrammer/render-markdown.nvim",
		ft = "markdown"
	},

	{
		"Mofiqul/vscode.nvim",
		priority = 1000,
		lazy = false,
		config = function()
			vim.cmd[[
				colorscheme vscode
				highlight Normal guibg=NONE ctermbg=NONE
				highlight EndOfBuffer guibg=NONE ctermbg=NONE
				highlight nonText guibg=NONE ctermbg=NONE
			]]
		end
	}
})

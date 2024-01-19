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
	require("plugins/config/lsp"),
	require("plugins/config/telescope"),
	require("plugins/config/harpoon"),
	require("plugins/config/cmp"),
	require("plugins/config/whichkey"),
	require("plugins/config/treesitter"),
	require("plugins/config/git"),
	require("plugins/config/colorizer"),
	require("plugins/config/bufferline"),
	require("plugins/config/statusline"),
	require("plugins/config/oil"),
	require("plugins/config/terminal"),

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
		"entrez/roku.vim",
		ft = "brs"
	},

	{
		"nvimtools/none-ls.nvim",
		event = "InsertEnter",
		config = function ()
			local null_ls = require("null-ls")
			null_ls.setup({
				sources = {
					null_ls.builtins.diagnostics.typos,
				}
			})
		end
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

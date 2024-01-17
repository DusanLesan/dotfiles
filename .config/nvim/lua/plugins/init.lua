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
	{
		'neovim/nvim-lspconfig',
		config = function()
			require("plugins/config/lsp")
		end,
		dependencies = {
			{
				'williamboman/mason.nvim',
				cmd = { "Mason", "MasonInstall", "MasonInstallAll", "MasonUpdate" },
				config = true
			},
			'williamboman/mason-lspconfig.nvim'
		}
	},

	{
		'nvim-telescope/telescope.nvim',
		cmd = "Telescope",
		opts = require("plugins/config/telescope"),
		dependencies = {
			'nvim-lua/plenary.nvim'
		}
	},

	{
		'hrsh7th/nvim-cmp',
		event = "InsertEnter",
		config = function()
			require("plugins/config/cmp")
		end,

		dependencies = {
			"hrsh7th/cmp-nvim-lsp",
			"hrsh7th/cmp-nvim-lua",
			"hrsh7th/cmp-buffer",
			"hrsh7th/cmp-path",
			"hrsh7th/vim-vsnip",
			"hrsh7th/cmp-vsnip",
			"hrsh7th/vim-vsnip-integ"
		}
	},

	{
		"folke/which-key.nvim",
		event= "VeryLazy",
		opts = require("plugins/config/whichkey"),
		config = function(_, opts)
			local wk = require("which-key")
			wk.setup(opts)
			wk.register(opts.defaults)
		end,
	},

	{
		"nvim-treesitter/nvim-treesitter",
		event = "VeryLazy",
		cmd = { "TSInstall", "TSBufEnable", "TSBufDisable", "TSModuleInfo" },
		opts = require("plugins/config/treesitter")
	},

	{
		"lewis6991/gitsigns.nvim",
		event = "VeryLazy",
		opts = require("plugins/config/git"),
		ft = { "gitcommit", "diff" }
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
		"lukas-reineke/indent-blankline.nvim",
		event = "InsertEnter",
		main = "ibl",
		opts = require("plugins/config/indentline")
	},

	{
		"NvChad/nvim-colorizer.lua",
		event = "VeryLazy",
		opts = require("plugins/config/colorizer")
	},

	{
		"akinsho/bufferline.nvim",
		event = "VeryLazy",
		opts = require("plugins/config/bufferline")
	},

	{
		"nvim-lualine/lualine.nvim",
		event = "VeryLazy",
		config = function()
			 require("lualine").setup(require("plugins/config/statusline"))
		end
	},

	{
		"akinsho/toggleterm.nvim",
		cmd = { "ToggleTerm" },
		opts = require("plugins/config/terminal")
	},

	{
		"entrez/roku.vim",
		ft = "brs"
	},

	{
		"nvim-tree/nvim-tree.lua",
		cmd = { "NvimTreeToggle", "NvimTreeFocus" },
		opts = require("plugins/config/nvimtree")
	},

	{
		"Mofiqul/vscode.nvim",
		priority = 1000,
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

local fn = vim.fn

-- Automatically install packer
local install_path = fn.stdpath "data" .. "/site/pack/packer/start/packer.nvim"
if fn.empty(fn.glob(install_path)) > 0 then
	PACKER_BOOTSTRAP = fn.system {
		"git",
		"clone",
		"--depth",
		"1",
		"https://github.com/wbthomason/packer.nvim",
		install_path,
	}
	print "Installing packer close and reopen Neovim..."
	vim.cmd [[packadd packer.nvim]]
end

-- Use a protected call so we don't error out on first use
local status_ok, packer = pcall(require, "packer")
if not status_ok then
	return
end

-- Have packer use a popup window
packer.init {
	display = {
		open_fn = function()
			return require("packer.util").float { border = "rounded" }
		end,
	},
}

vim.opt.termguicolors = true

return require("packer").startup(function(use)
	-- PACKER
	use { "wbthomason/packer.nvim" }

	-- LSP
	use { "neovim/nvim-lspconfig" }
	use { "L3MON4D3/LuaSnip" }
	use { "williamboman/nvim-lsp-installer" }

	-- CMP
	use { "hrsh7th/nvim-cmp" }
	use { "hrsh7th/cmp-nvim-lua" }
	use { "hrsh7th/cmp-nvim-lsp" }
	use { "hrsh7th/cmp-buffer" }
	use { "hrsh7th/cmp-path" }
	use { "saadparwaiz1/cmp_luasnip" }

	-- Whichkey
	use { "folke/which-key.nvim" }

	-- Tab, Statusline, Indintline
	use { "akinsho/bufferline.nvim" }
	use { "nvim-lualine/lualine.nvim" }
	use { "lukas-reineke/indent-blankline.nvim" }
	use { "Asheq/close-buffers.vim" }

	-- comment
	use { "winston0410/commented.nvim" }

	-- TERM
	use {"akinsho/toggleterm.nvim"}

	-- Files
	use { "kyazdani42/nvim-tree.lua" }

	-- Telescope
	use { "nvim-lua/plenary.nvim" }
	use { "nvim-telescope/telescope.nvim" }

	-- Dashboard
	use { "glepnir/dashboard-nvim" }

	-- Git
	use { "lewis6991/gitsigns.nvim" }

	-- AUTOPAIRS
	use { "windwp/nvim-autopairs" }
	use { "andymass/vim-matchup" }

	-- Colors and treesitter
	use { "dusanlesan/vimbrs" }
	use { "norcalli/nvim-colorizer.lua" }
	use { "nvim-treesitter/nvim-treesitter" }
	use { "Mofiqul/vscode.nvim" }

	if PACKER_BOOTSTRAP then
		require("packer").sync()
	end
end)


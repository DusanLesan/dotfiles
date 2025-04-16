local M = 	{
	"nvim-treesitter/nvim-treesitter",
	event = "VeryLazy"
}

local opts = {
	use_languagetree = true,
	ensure_installed = {
		"bash",
		"lua",
		"c",
		"cpp",
		"css",
		"java",
		"json",
		"yaml",
		"http",
		"brightscript"
	},
	highlight = {
		enable = true,
		matchup = {
			enable = true
		},
		disable = function(_, bufnr)
			return vim.api.nvim_buf_line_count(bufnr) > 10000
		end,
	},
	context_commentstring = {
		enable = true,
		enable_autocmd = true
	},
	incremental_selection = {
		enable = true,
		keymaps = {
			init_selection = "<CR>",
			node_incremental = "<CR>",
			scope_incremental = "<leader>gi",
			node_decremental = "<BS>"
		}
	}
}

function M.config()
	vim.treesitter.language.register('brightscript', 'brs')
	require("nvim-treesitter.configs").setup(opts)
end

return M

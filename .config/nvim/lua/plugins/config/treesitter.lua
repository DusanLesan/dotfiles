local M = 	{
	"nvim-treesitter/nvim-treesitter",
	event = "VeryLazy"
}

local opts = {
	use_languagetree = true,
	ensure_installed = {
	},
	highlight = {
		enable = true,
		matchup = {
			enable = true
		},
		disable = function(lang, buf)
			local max_filesize = 100 * 1024 -- 100 KB
			local ok, stats = pcall(vim.loop.fs_stat, vim.api.nvim_buf_get_name(buf))
			if ok and stats and stats.size > max_filesize then
				return true
			end
		end,
	},
	context_commentstring = {
		enable = true,
		enable_autocmd = true
	},
	incremental_selection = {
		enable = true,
		keymaps = {
			init_selection = "<leader>ss",
			node_incremental = "<leader>si",
			scope_incremental = "<leader>sc",
			node_decremental = "<leader>sd"
		}
	}
}

function M.config()
	require("nvim-treesitter.configs").setup(opts)
end

return M

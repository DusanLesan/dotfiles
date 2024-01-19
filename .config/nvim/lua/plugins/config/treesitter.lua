local M = 	{
	"nvim-treesitter/nvim-treesitter",
	event = "VeryLazy",
	cmd = { "TSInstall", "TSBufEnable", "TSBufDisable", "TSModuleInfo" }
}

M.opts = {
	use_languagetree = true,
	ensure_installed = {
		"bash",
		"lua",
		"c",
		"cpp",
		"css",
		"java",
		"json",
		"yaml"
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
	}
}

return M

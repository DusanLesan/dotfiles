local status_ok, ts_config = pcall(require, "nvim-treesitter.configs")
if not status_ok then
	return
end

ts_config.setup {
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
		}
	},
	context_commentstring = {
		enable = true,
		enable_autocmd = true
	}
}

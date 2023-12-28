local status_ok, telescope = pcall(require, "telescope")
if not status_ok then
	return
end

telescope.setup {
	defaults = {
		vimgrep_arguments = {
			"rg",
			"--color=never",
			"--no-heading",
			"--with-filename",
			"--line-number",
			"--column",
			"--smart-case",
			"--hidden"
		},
		lsp = {
			symbol_conflict_lsp_method = 'locationlist',
			symbol_query_config = {
				debounce = 100,
				batch_size = 256,
				args = {
					"--kind", "Function", "--regex"
				}
			}
		},
		file_cache = true,
		path_display = { "smart" },
		file_ignore_patterns = { "node_modules", "cicd/automation" }
	}
}

pcall(telescope.load_extension, 'fzf')

local M = {
	'nvim-telescope/telescope.nvim',
	cmd = "Telescope",
	dependencies = {
		'nvim-lua/plenary.nvim'
	}
}

function M.config()
	local telescope = require("telescope")
	telescope.setup({
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
			file_ignore_patterns = { "node_modules", "cicd/automation" },
			mappings = {
				n = {
					['<c-d>'] = require('telescope.actions').delete_buffer
				},
				i = {
					["<C-h>"] = "which_key",
					['<c-d>'] = require('telescope.actions').delete_buffer
				}
			}
		}
	})
end

return M

local parsers = {
	"bash",
	"lua",
	"c",
	"cpp",
	"css",
	"java",
	"json",
	"yaml",
	"http",
	"brightscript",
}

return {
	{
		"nvim-treesitter/nvim-treesitter",
		lazy = false,
		build = ":TSUpdate",
		config = function()
			local ts = require("nvim-treesitter")
			ts.setup({
				install_dir = vim.fn.stdpath("data") .. "/site",
			})

			vim.treesitter.language.register("brightscript", "brs")
			require("ts_context_commentstring").setup({
				enable_autocmd = true,
			})

			ts.install(parsers)

			vim.api.nvim_create_autocmd("FileType", {
				group = vim.api.nvim_create_augroup("my.treesitter", { clear = true }),
				pattern = "*",
				callback = function(args)
					if vim.api.nvim_buf_line_count(args.buf) > 10000 then return end
					pcall(vim.treesitter.start, args.buf)
				end,
			})
		end,
		dependencies = {
			"JoosepAlviste/nvim-ts-context-commentstring",
		},
	},
	{
		"nvim-treesitter/nvim-treesitter-textobjects",
		branch = "main",
		config = function()
			require("nvim-treesitter-textobjects").setup({
				move = { set_jumps = true },
				select = {
					lookahead = false,
					lookbehind = false,
					include_surrounding_whitespace = false,
				},
			})
		end,
	},
	{
		"MeanderingProgrammer/treesitter-modules.nvim",
		dependencies = { "nvim-treesitter/nvim-treesitter" },
		opts = {
			incremental_selection = {
				enable = true,
				keymaps = {
					init_selection = '<CR>',
					node_incremental = '<CR>',
					scope_incremental = false,
					node_decremental = '<BS>',
				},
			},
		},
	},
}

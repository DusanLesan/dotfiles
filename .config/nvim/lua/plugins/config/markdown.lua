local colors = require("colors")

local M = {
	"MeanderingProgrammer/render-markdown.nvim",
	ft = "markdown",
	cond = vim.g.vscode == nil,
}

local render_markdown_code = { bg = colors.bg_secondary, default = false }

function M.config()
	require("render-markdown").setup({
		code = {
			position = "right",
			language_info = false,
			highlight_language = "Special",
			width = "block",
			right_pad = 1,
			min_width = 24,
		},
		win_options = {
			list = { default = vim.o.list, rendered = false },
		},
	})
	vim.api.nvim_create_autocmd("ColorScheme", {
		callback = function()
			vim.api.nvim_set_hl(0, "RenderMarkdownCode", render_markdown_code)
		end,
	})
	vim.api.nvim_set_hl(0, "RenderMarkdownCode", render_markdown_code)
end

return M

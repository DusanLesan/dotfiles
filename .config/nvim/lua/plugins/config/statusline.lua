local M = {
	"nvim-lualine/lualine.nvim",
	event = "VeryLazy"
}

local colors = require("colors")

local conditions = {
	buffer_not_empty = function()
		return vim.fn.empty(vim.fn.expand '%:t') ~= 1
	end,
	hide_in_width = function()
		return vim.fn.winwidth(0) > 80
	end,
	check_git_workspace = function()
		local filepath = vim.fn.expand '%:p:h'
		local gitdir = vim.fn.finddir('.git', filepath .. ';')
		return gitdir and #gitdir > 0 and #gitdir < #filepath
	end
}

local opts = {
	options = {
		-- Disable sections and separators
		component_separators = '',
		section_separators = '',
		theme = {
			normal = { c = { fg = colors.fg, bg = colors.bg } },
			inactive = { c = { fg = colors.fg, bg = colors.bg } }
		}
	},
	sections = {
		-- Remove defaults
		lualine_a = {}, lualine_b = {}, lualine_y = {}, lualine_z = {},
		-- All modules will be placed in _c and _x
		lualine_c = {}, lualine_x = {}
	}
}

-- Inserts a component in lualine_c at left section
local function ins_left(component)
	table.insert(opts.sections.lualine_c, component)
end

-- Inserts a component in lualine_x at right section
local function ins_right(component)
	table.insert(opts.sections.lualine_x, component)
end

ins_left('mode')

ins_left({
	'branch',
	color = { fg = colors.violet, gui = 'bold' }
})

ins_left({
	'diff',
	symbols = { added = '+', modified = '<>', removed = '-' },
	diff_color = {
		added = { fg = colors.green },
		modified = { fg = colors.orange },
		removed = { fg = colors.red }
	},
	cond = conditions.hide_in_width
})

ins_left({
	'filesize',
	cond = conditions.buffer_not_empty
})

ins_left({
	'filename',
	cond = conditions.buffer_not_empty,
	color = { fg = colors.magenta, gui = 'bold' }
})

ins_left({
	'macro',
	fmt = function()
		local reg = vim.fn.reg_recording()
		if reg ~= "" then
			return "Recording @" .. reg
		end
		return nil
	end,
	color = { fg = "#ff9e64" },
	draw_empty = false,
})

ins_right({ 'selectioncount' })
ins_right({ 'searchcount' })
ins_right({ 'progress', color = { fg = colors.fg, gui = 'bold' } })

ins_right({
	'diagnostics',
	sources = { 'nvim_diagnostic' },
	symbols = { error = 'X', warn = '!', info = '!' },
	diagnostics_color = {
		color_error = { fg = colors.red },
		color_warn = { fg = colors.yellow },
		color_info = { fg = colors.cyan }
	}
})

local color = colors.red
ins_right({
	function()
		local buf_ft = vim.bo.filetype
		local clients = vim.lsp.get_clients()

		if next(clients) == nil then
			return buf_ft
		end
		for _, client in ipairs(clients) do
			local filetypes = client.config.filetypes
			if filetypes and vim.fn.index(filetypes, buf_ft) ~= -1 then
				color = colors.green
				return client.name
			end
		end

		return buf_ft
	end,
	color = function()
		return { fg = color, gui = 'bold' }
	end
})

function M.config()
	require("lualine").setup(opts)
end

return M

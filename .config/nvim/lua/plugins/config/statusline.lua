local lsp = require('feline.providers.lsp')
local vi_mode_utils = require('feline.providers.vi_mode')

local force_inactive = {
		filetypes = {},
		buftypes = {},
		bufnames = {}
}

local components = {
	active = {{}, {}, {}},
	inactive = {{}, {}, {}}
}

local colors = require("colors")

local vi_mode_colors = {
	NORMAL = 'green',
	OP = 'green',
	INSERT = 'red',
	VISUAL = 'blue',
	BLOCK = 'blue',
	REPLACE = 'violet',
	['V-REPLACE'] = 'violet',
	ENTER = 'cyan',
	MORE = 'cyan',
	SELECT = 'orange',
	COMMAND = 'green',
	SHELL = 'green',
	TERM = 'green',
	NONE = 'yellow'
}

local vi_mode_text = {
	n = "NORMAL",
	i = "INSERT",
	v = "VISUAL",
	[''] = "V-BLOCK",
	V = "V-LINE",
	c = "COMMAND",
	no = "UNKNOWN",
	s = "UNKNOWN",
	S = "UNKNOWN",
	ic = "UNKNOWN",
	R = "REPLACE",
	Rv = "UNKNOWN",
	cv = "UNKWON",
	ce = "UNKNOWN",
	r = "REPLACE",
	rm = "UNKNOWN",
	t = "INSERT"
}

force_inactive.filetypes = {
	'NvimTree',
	'dbui',
	'packer',
	'startify',
	'dashboard',
	'fugitive',
	'fugitiveblame'
}

-- LEFT
-- vi-mode
components.active[1][1] = {
	provider = function()
		return ' '.. vi_mode_text[vim.fn.mode()]
	end,
	hl = function()
		local val = {}
		val.fg = vi_mode_utils.get_mode_color()
		val.bg = 'bg'
		val.style = 'bold'
		return val
	end,
	right_sep = ' '
}
-- filename
components.active[1][2] = {
	provider = function()
		return vim.fn.expand("%:F")
	end,
	hl = {
		fg = 'white',
		bg = 'bg',
		style = 'bold'
	},
	right_sep = '  '
}
-- gitBranch
components.active[1][3] = {
	provider = 'git_branch',
	hl = {
		fg = 'yellow',
		bg = 'bg',
		style = 'bold'
	}
}
-- diffAdd
components.active[1][4] = {
	provider = 'git_diff_added',
	hl = {
		fg = 'green',
		bg = 'bg',
		style = 'bold'
	}
}
-- diffModfified
components.active[1][5] = {
	provider = 'git_diff_changed',
	hl = {
		fg = 'orange',
		bg = 'bg',
		style = 'bold'
	}
}
-- diffRemove
components.active[1][6] = {
	provider = 'git_diff_removed',
	hl = {
		fg = 'red',
		bg = 'bg',
		style = 'bold'
	}
}

-- MID
-- LspName
components.active[3][1] = {
	provider = 'lsp_client_names',
	hl = {
		fg = 'white',
		bg = 'bg',
		style = 'bold'
	},
	right_sep = ' '
}
-- diagnosticErrors
components.active[3][2] = {
	provider = 'diagnostic_errors',
	enabled = function() return lsp.diagnostics_exist('Error') end,
	hl = {
		fg = 'red',
		style = 'bold'
	}
}
-- diagnosticWarn
components.active[3][3] = {
	provider = 'diagnostic_warnings',
	enabled = function() return lsp.diagnostics_exist('Warning') end,
	hl = {
		fg = 'yellow',
		style = 'bold'
	}
}
-- diagnosticHint
components.active[3][4] = {
	provider = 'diagnostic_hints',
	enabled = function() return lsp.diagnostics_exist('Hint') end,
	hl = {
		fg = 'cyan',
		style = 'bold'
	}
}
-- diagnosticInfo
components.active[3][5] = {
	provider = 'diagnostic_info',
	enabled = function() return lsp.diagnostics_exist('Information') end,
	hl = {
		fg = 'skyblue',
		style = 'bold'
	}
}

-- RIGHT
-- lineInfo
components.active[3][6] = {
	provider = 'position',
	hl = {
		fg = 'white',
		bg = 'bg',
		style = 'bold'
	},
	right_sep = ' '
}
-- scrollBar
components.active[3][7] = {
	provider = 'scroll_bar',
	hl = {
		fg = 'yellow',
		bg = 'bg',x
	}
}

require('feline').setup({
	colors = colors,
	vi_mode_colors = vi_mode_colors,
	components = components,
	force_inactive = force_inactive
})

local status_ok, toggleterm = pcall(require, "toggleterm")
if not status_ok then
	return
end

local Terminal = require('toggleterm.terminal').Terminal
toggleterm.setup({
	on_config_done = nil,
	size = 20,
	open_mapping = [[<c-t>]],
	hide_numbers = true,
	shade_filetypes = {},
	shade_terminals = true,
	shading_factor = 2,
	start_in_insert = true,
	insert_mappings = true,
	persist_size = false,
	direction = "float",
	close_on_exit = true,
	shell = vim.o.shell,
	float_opts = {
		border = "curved",
		winblend = 0,
		highlights = {
			border = "Normal",
			background = "Normal"
		}
	}
})

-- lazygit
local lazygit = Terminal:new({ cmd = "lazygit", hidden = true })
function _lazygit_toggle()
	lazygit:toggle()
end

-- htop
local htop = Terminal:new({ cmd = "htop", hidden = true })
function _htop_toggle()
	htop:toggle()
end

-- Python
local python = Terminal:new({ cmd = "python", hidden = true })
function _python_toggle()
	python:toggle()
end

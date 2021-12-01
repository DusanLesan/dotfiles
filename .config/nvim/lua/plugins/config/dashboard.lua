local g = vim.g

local plugins_count = vim.fn.len(vim.fn.globpath("~/.local/share/nvim/site/pack/packer/start", "*", 0, 1))
g.dashboard_default_executive = "telescope"

g.dashboard_disable_statusline = 0
g.dashboard_custom_footer = {
	"Loaded " .. plugins_count .. " plugins  "
}

g.dashboard_custom_section = {
	a = { description = { "  Find File        " }, command = "Telescope find_files" },
	c = { description = { "  Recents          " }, command = "Telescope oldfiles" },
	d = { description = { "  Colorschemes     " }, command = "Telescope colorscheme" },
	e = { description = { "  New File         " }, command = "DashboardNewFile" },
	f = { description = { "煉 Settings         " }, command = "e ~/.config/nvim/lua/nv-config.lua" },
	g = { description = { "  Load Last Session" }, command = "SessionLoad" }
}

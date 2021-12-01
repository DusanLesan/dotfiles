vim.api.nvim_command "hi clear"
if vim.fn.exists "syntax_on" then
	vim.api.nvim_command "syntax reset"
end

local colors = {
	notify_bg = 'Normal',
	white = "#abb2bf",
	black = "#1e1e1e",
	bg = "#252526",
	fg = "#bbc2cf",
	yellow = "#ECBE7B",
	cyan = "#008080",
	green = "#98be65",
	orange = "#FF8800",
	violet = "#a9a1e1",
	magenta = "#c678dd",
	purple = "#c678dd",
	pink = "#ff1493",
	blue = "#51afef",
	teal = "#008080",
	grey = "#4d5a5e",
	red = "#ec5f67",
	black2 = "#2d2d2d",
	one_bg3 = "#30343c",
	grey_fg = "#565c64",
	light_grey = "#6f737b",
	nord_blue = "#81A1C1"
}

return colors

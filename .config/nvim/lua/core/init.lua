vim.cmd("set shortmess+=c")
vim.cmd("set nofoldenable")
vim.cmd("set laststatus=0")

vim.opt.history = 100
vim.opt.synmaxcol = 240
vim.opt.shiftwidth = 4
vim.opt.tabstop = 4
vim.opt.pumheight = 10
vim.opt.timeout = true
vim.opt.cursorline = true
vim.opt.timeoutlen = 500
vim.opt.completeopt = "menuone,noselect"

vim.opt.wrap = true
vim.opt.backup = false
vim.opt.swapfile = false
vim.opt.termguicolors = true
vim.opt.smartindent = true
vim.opt.smartcase = true
vim.opt.ignorecase = true
vim.opt.hidden = true
vim.opt.lazyredraw = true
vim.opt.showmatch = true
vim.opt.splitbelow = true
vim.opt.splitright = true
vim.opt.number = true
vim.opt.relativenumber = true
vim.opt.whichwrap:append "<>[]hl"

vim.opt.cmdheight = 0
vim.opt.mouse = 'a'
vim.opt.clipboard = 'unnamedplus'

vim.opt.list = true
vim.opt.listchars:append("space:⋅")

vim.g.mapleader = ' '
vim.g.transparent_background = true
vim.g.vscode_style = 'dark'

local trim_whitespace = function()
	vim.cmd([[
		keeppatterns
		%s/\s\+$//e |
		%s/\n\{3,}/\r\r/e |
		%s/\n\+\%$//e |
		%s/\%$[^\n]/&\r/e
	]])
end

vim.api.nvim_create_autocmd('BufWritePre', {
	group = vim.api.nvim_create_augroup('trim_whitespaces', { clear = true }),
	pattern = '*',
	callback = trim_whitespace,
})

vim.api.nvim_create_autocmd("FileType", {
	pattern = "json",
	callback = function()
		vim.opt_local.formatprg = "jq"
	end
})

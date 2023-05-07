
vim.cmd("let g:matchup_matchparen_offscreen = {'method': 'popup'}")
vim.cmd("set shortmess+=c")
vim.cmd("let g:loaded_matchit = 1")
vim.cmd("set nofoldenable")

vim.opt.history = 100
vim.opt.synmaxcol = 240
vim.opt.shiftwidth = 4
vim.opt.tabstop = 4
vim.opt.pumheight = 10
vim.opt.timeoutlen = 150
vim.opt.completeopt = "menuone,noselect"

vim.g.loaded_matchit = 1
vim.g.matchup_delim_stopline = 1500
vim.g.matchup_matchparen_stopline = 400

vim.opt.wrap = true
vim.opt.showmode = false
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

vim.opt.cmdheight = 1
vim.opt.mouse = 'a'
vim.opt.clipboard = 'unnamedplus'
vim.g.mapleader = ' '

vim.g.transparent_background = true
vim.g.vscode_style = 'dark'

local status_ok, _ = pcall(require, "vscode")
if status_ok then
	vim.cmd[[
		colorscheme vscode
		highlight Normal guibg=NONE ctermbg=NONE
		highlight EndOfBuffer guibg=NONE ctermbg=NONE
		highlight nonText guibg=NONE ctermbg=NONE
	]]
end

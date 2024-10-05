vim.cmd("set shortmess+=c")
vim.cmd("set nofoldenable")
vim.cmd("set laststatus=0")

vim.opt.history = 100
vim.opt.synmaxcol = 240
vim.opt.shiftwidth = 4
vim.opt.tabstop = 4
vim.opt.pumheight = 10
vim.opt.timeout = true
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

vim.opt.cmdheight = 1
vim.opt.mouse = 'a'
vim.opt.clipboard = 'unnamedplus'

vim.opt.list = true
vim.opt.listchars:append("space:⋅")

vim.g.mapleader = ' '
vim.g.transparent_background = true
vim.g.vscode_style = 'dark'

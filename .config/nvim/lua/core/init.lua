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

vim.filetype.add({
	extension = {
		bookmarks = "bookmarks",
		brs = 'brs'
	}
})

local trim_whitespace = function()
	vim.cmd([[
		keeppatterns
		%s/\s\+$//e |
		%s/\n\{3,}/\r\r/e |
		%s/\n\+\%$//e |
		%s/\%$[^\n]/&\r/e
	]])
end

local group = vim.api.nvim_create_augroup("user_autocmds", { clear = true })

vim.api.nvim_create_autocmd("BufWritePre", {
	group = group,
	pattern = "*",
	callback = trim_whitespace
})

vim.api.nvim_create_autocmd("FileType", {
	group = group,
	pattern = { "json", "brs", "python", "bookmarks", "http" },
	callback = function(args)
		local ft = args.match
		if ft == "json" then
			vim.opt_local.formatprg = "jq"
		elseif ft == "brs" then
			vim.opt_local.commentstring = "' %s"
		elseif ft == "python" then
			vim.opt_local.expandtab = false
		elseif ft == "bookmarks" then
			vim.keymap.set("n", "<CR>", function()
				local pattern, path = vim.api.nvim_get_current_line():match("^%s*'(.-)'%s+(.+)$")
				if not (pattern and path) then return end
				vim.cmd("edit " .. path)
				if tonumber(pattern) then
					vim.cmd(pattern)
				else
					vim.fn.search(pattern)
				end
			end, { buffer = true, desc = "Jump to bookmarked pattern in file" })
		elseif ft == "http" then
			vim.opt_local.expandtab = true
			vim.schedule(function()
				vim.keymap.set("n", "<CR>", ":Rest run<CR>", { buffer = true, desc = "Run HTTP request" })
			end)
		end
	end
})

vim.api.nvim_create_autocmd("BufEnter", {
	group = group,
	callback = function()
		local git_root = vim.fn.systemlist("git rev-parse --show-toplevel")[1]
		if git_root and vim.fn.isdirectory(vim.fn.trim(git_root)) == 1 then
			vim.cmd("cd " .. vim.fn.fnameescape(git_root))
		end
	end,
})

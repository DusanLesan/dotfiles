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
	},
	pattern = {
		[".*lfrc.*"] = "lf",
	},
})

local function get_project_root()
	local util = require("lspconfig.util")
	local fname = vim.api.nvim_buf_get_name(0)
	if fname == "" then return nil end
	return util.root_pattern("Makefile")(fname)
		or util.root_pattern("compile_commands.json", "compile_flags.txt")(fname)
		or util.find_git_ancestor(fname)
end

local function get_bookmark_file()
	local root = get_project_root()
	if not root then
		vim.notify("Could not determine project root.", vim.log.levels.ERROR)
		return nil
	end

	local hash = vim.fn.sha256(root)
	local bookmark_file = vim.fn.stdpath("data") .. "/bookmarks/" .. hash .. ".bookmarks"
	vim.fn.mkdir(vim.fn.fnamemodify(bookmark_file, ":h"), "p")
	return bookmark_file
end

vim.api.nvim_create_user_command("OpenProjectBookmarks", function()
	local bookmark_file = get_bookmark_file()
	if not bookmark_file then return end
	vim.cmd("edit " .. vim.fn.fnameescape(bookmark_file))
end, {})

vim.api.nvim_create_user_command("OpenProjectBookmarks", function()
	local root = get_project_root()
	if not root then
		vim.notify("Could not determine project root.", vim.log.levels.ERROR)
		return
	end

	local hash = vim.fn.sha256(root)
	local path = vim.fn.stdpath("data") .. "/bookmarks/" .. hash .. ".bookmarks"

	vim.fn.mkdir(vim.fn.fnamemodify(path, ":h"), "p")
	vim.cmd("edit " .. vim.fn.fnameescape(path))
end, {})

vim.api.nvim_create_user_command("AddBookmark", function()
	local bookmark_file = get_bookmark_file()
	if not bookmark_file then return end
	local line_content = vim.fn.getline(".")
	local filepath = vim.fn.expand("%:p")
	line_content = line_content:gsub("'", "\\'")
	local entry = string.format("'%s' %s\n", line_content, filepath)
	local f = io.open(bookmark_file, "a")
	if not f then return end
	f:write(entry)
	f:close()
	vim.notify("Added to bookmarks: " .. entry)
end, {})

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

vim.api.nvim_create_user_command("SwitchPairedFile", function()
	local cur = vim.api.nvim_buf_get_name(0)
	local dir, base = vim.fn.fnamemodify(cur, ":h"), vim.fn.fnamemodify(cur, ":t:r")
	for _, f in ipairs(vim.fn.globpath(dir, base .. ".*", false, true)) do
		if f ~= cur then return vim.cmd.edit(vim.fn.fnameescape(f)) end
	end
	print("No paired file found.")
end, {})

vim.api.nvim_create_user_command("OpenInLf", function()
	local path = vim.fn.expand("<cfile>")
	if vim.fn.filereadable(path) == 1 or vim.fn.isdirectory(path) == 1 then
		vim.fn.jobstart({ "env", "_START_LFCD=" .. path, "alacritty" }, { detach = true })
	else
		vim.fn.jobstart({ "notify-send", "Error", "Invalid path: " .. path })
	end
end, {})

vim.api.nvim_create_user_command("EvalMath", function()
	vim.cmd('normal! gv"vy')
	local f, err = load("return " .. vim.fn.getreg("v"))
	if not f then return vim.notify("Invalid expression: " .. err, vim.log.levels.ERROR) end
	local ok, result = pcall(f)
	if ok then vim.cmd('normal! gv"vc' .. result) end
end, { range = true })

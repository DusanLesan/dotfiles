local map = vim.keymap.set
local function mmap(mode, lhs, rhs, opts)
	for _, keys in ipairs(lhs) do
		map(mode, keys, rhs, opts)
	end
end

local function desc(description)
	return { noremap = true, silent = true, desc = description }
end

mmap('n', {'<C-S-Up>', '<C-K>'}, ':m .-2<CR>', desc('Move line up'))
mmap('n', {'<C-S-Down>', '<C-J>'}, ':m .+1<CR>', desc('Move line down'))

map('i', '<C-BS>', '<C-w>', desc('Delete word left'))
map('i', '<C-Delete>', '<C-o>dw', desc('Delete word right'))

map('n', '<F10>', ':set spell!<CR>', desc('Toggle spell check'))

map('v', '<', '<gv', desc('Indent left'))
map('v', '>', '>gv', desc('Indent right'))

map('x', 'p', [["_dP]], desc('Paste over selection preserving register'))
map({'n', 'v'}, '<A-d>', '"_d', desc('Delete into black hole'))
map({'n', 'v'}, '<leader>d', '"Ad', desc('Delete into "a" register'))
map({'n', 'v'}, '<leader>y', '"Ay', desc('Yank into "a" register'))
map({'n', 'v'}, '<leader>p', '"ap', desc('Paste from "a" register'))
map('n', '<leader>c', ':let @a=""<CR>', desc('Clear "a" register'))

-- Window controls
mmap('n', {'<A-Up>', '<A-k>'}, '<C-w>k', desc('Focus window above'))
mmap('n', {'<A-Down>', '<A-j>'}, '<C-w>j', desc('Focus window below'))
mmap('n', {'<A-Left>', '<A-h>'}, '<C-w>h', desc('Focus window left'))
mmap('n', {'<A-Right>', '<A-l>'}, '<C-w>l', desc('Focus window right'))
map('t', '<A-Up>', '<C-\\><C-N><C-w>k', desc('Focus window above'))
map('t', '<A-Down>', '<C-\\><C-N><C-w>j', desc('Focus window below'))
map('t', '<A-Left>', '<C-\\><C-N><C-w>h', desc('Focus window left'))
map('t', '<A-Right>', '<C-\\><C-N><C-w>l', desc('Focus window right'))

-- Window resizing
mmap('n', {'<C-A-Up>', '<C-A-k>'}, ':resize -2<CR>', desc('Resize window up'))
mmap('n', {'<C-A-Down>', '<C-A-j>'}, ':resize +2<CR>', desc('Resize window down'))
mmap('n', {'<C-A-Left>', '<C-A-h>'}, ':vertical resize -2<CR>', desc('Resize window left'))
mmap('n', {'<C-A-Right>', '<C-A-l>'}, ':vertical resize +2<CR>', desc('Resize window right'))

-- Buffers
map('n', '<TAB>', ':BufferLineCycleNext<CR>', desc('Next buffer'))
map('n', '<S-TAB>', ':BufferLineCyclePrev<CR>', desc('Previous buffer'))
map('n', '<leader>z', ':bdelete<CR>', desc('Close buffer'))
map('n', '<A-z>', ':bdelete!<CR>', desc('Force close buffer'))

map('n', '<leader>e', ":Oil<CR>", desc('Open file manager'))
map('n', '<A-t>', ':ToggleTerm<CR>', desc('Toggle terminal'))

map('n', '<leader>w', ':w<CR>', desc('Save file'))
map('n', '<leader>x', ':wqa!<CR>', desc('Save and quit'))

map ('n', '<leader>fr', ':Rest run<CR>', desc('Execute rest request'))

map('n', '<leader>fc', ':Telescope colorscheme<CR>', desc('Select colorscheme'))

map('n', '<leader>ff', ':Telescope find_files<CR>', desc('Find files'))

-- Lsp
map('n', '<leader>K', '<cmd>lua vim.lsp.buf.hover()<CR>', desc('Hover'))
map('n', '<C-k', '<cmd>lua vim.lsp.buf.signature_help()<CR>', desc('Signature help'))
map('n', 'gd', '<cmd>lua vim.lsp.buf.definition()<CR>', desc('Go to definition'))
map('n', 'gD', '<cmd>lua vim.lsp.buf.declaration()<CR>', desc('Go to declaration'))
map('n', 'gi', '<cmd>lua vim.lsp.buf.implementation()<CR>', desc('Go to implementation'))
map('n', 'gr', '<cmd>lua vim.lsp.buf.references()<CR>', desc('References'))
map('n', 'gt', '<cmd>lua vim.lsp.buf.type_definition()<CR>', desc('Type definition'))
map('n', 'ga', '<cmd>lua vim.lsp.buf.code_action()<CR>', desc('Code action'))
map('n', '<leader>lr', '<cmd>lua vim.lsp.buf.rename()<CR>', desc('Rename symbol'))
map('n', '<leader>li', '<cmd>lua vim.lsp.buf.incoming_calls()<CR>', desc('Incoming calls'))
map('n', ']d', '<cmd>lua vim.diagnostic.goto_next()<CR>', desc('Next diagnostic'))
map('n', '[d', '<cmd>lua vim.diagnostic.goto_prev()<CR>', desc('Previous diagnostic'))
map('n', '<leader>ld', '<cmd>lua vim.diagnostic.open_float()<CR>', desc('Line diagnostics'))
map('n', '<leader>lq', '<cmd>lua vim.diagnostic.setloclist()<CR>', desc('Set loclist with diagnostics'))
map('n', '<leader>lx', '<cmd>lua vim.diagnostic.reset()<CR>', desc('Clear diagnostics'))
map('n', '<leader>lt', '<cmd>lua vim.diagnostic.config({ virtual_text = not vim.diagnostic.config().virtual_text })<CR>', desc('Toggle virtual text'))

map('n', '<leader>fs', ':Telescope lsp_document_symbols<CR>', desc('List symbols'))
map('n', '<leader>fS', ':Telescope lsp_workspace_symbols<CR>', desc('List workspace symbols'))
map('n', '<leader><leader>', ':Telescope git_files<CR>', desc('Find files'))
map('n', '<leader>fh', ':Telescope oldfiles<CR>', desc('List recently opened files'))
map('n', '<leader>fw', ':Telescope live_grep<CR>', desc('Search words'))
map('n', '<leader>fb', ':Telescope buffers<CR>', desc('List buffers'))
map('n', '<leader>fz', ':Telescope current_buffer_fuzzy_find<CR>', desc('Find in buffer'))

map('n', '<leader>S', ':lua require("spectre").toggle()<CR>', desc('Toggle Spectre'))
map('n', '<leader>sw', ':lua require("spectre").open_visual({select_word=true})<CR>', desc('Search current word'))
map('v', '<leader>sw', ':lua require("spectre").open_visual()<CR>', desc('Search current word'))
map('n', '<leader>sp', ':lua require("spectre").open_file_search({select_word=true})<CR>', desc('Search on current file'))

-- Git
map('n', '<leader>gr', ":Gitsigns reset_hunk<CR>", desc('Reset hunk'))
map('n', '<leader>gR', ":Gitsigns reset_buffer<CR>", desc('Reset buffer'))
map('n', '<leader>gs', ":Gitsigns stage_hunk<CR>", desc('Stage hunk'))
map('n', '<leader>gS', ":Gitsigns stage_buffer<CR>", desc('Stage buffer'))
map('n', '<leader>gu', ":Gitsigns undo_stage_hunk<CR>", desc('Undo stage hunk'))
map('n', '<leader>gl', ":Gitsigns toggle_current_line_blame<CR>", desc('Toggle blame'))
map('n', '<leader>gj', ":Gitsigns next_hunk<CR>", desc('Next hunk'))
map('n', '<leader>gk', ":Gitsigns prev_hunk<CR>", desc('Previous hunk'))
map('n', '<leader>gg', ":Gitsigns toggle_signs<CR>", desc('Toggle signs'))
map('n', '<leader>gt', ":Telescope git_status<CR>", desc('Git status'))
map('n', '<leader>gc', ":Telescope git_commits<CR>", desc('Git commits'))

map('n', '<leader>lc', ':! sudo make clean install<CR><CR>', desc('Execute make clean install'))
map("n", "<F5>",
	'<cmd>wa | silent !env $(grep -E "^(roku_pass|roku_device)=" "$XDG_DATA_HOME/secrets/priv") roku-build -r<CR>',
	desc("Build and deploy roku project"))

vim.api.nvim_create_autocmd('FileType', {
	pattern = 'qf',
	callback = function()
		map('n', '<CR>', '<CR>:cclose<CR>', { buffer = true, silent = true })
		mmap('n', {'j', '<Down>'}, 'j<CR><c-w>p', { buffer = true, silent = true })
		mmap('n', {'k', '<Up>'}, 'k<CR><c-w>p', { buffer = true, silent = true })
	end
})

map("n", "gF", function()
	local path = vim.fn.expand("<cfile>")
	if vim.fn.filereadable(path) == 1 or vim.fn.isdirectory(path) == 1 then
		vim.fn.jobstart({"env", "_START_LFCD=" .. path, "alacritty"}, { detach = true })
	else
		vim.fn.jobstart({"notify-send", "Error", "Invalid path: " .. path})
	end
end, desc("Open path below cursor in lf"))

map('n', '<A-T>', function()
	vim.fn.jobstart({ 'alacritty', '--working-directory', vim.fn.getcwd(), '-e', 'tmux' }, { detach = true })
end, desc('Open external terminal'))

map("v", "<leader>m", function()
	vim.cmd('normal! "vy')
	local f, err = load("return " .. vim.fn.getreg("v"))
	if not f then return vim.notify("Invalid expression: " .. err, vim.log.levels.ERROR) end
	local ok, result = pcall(f)
	if ok then vim.cmd('normal! gv"vc' .. result) end
end, desc("Evaluate visual math expression"))

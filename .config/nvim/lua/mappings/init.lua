local map = vim.api.nvim_set_keymap
local default_opts = {noremap = true, silent = true}

-- Move lines up and down with alt-up/down
map('n', '<A-k>', ':m .-2<CR>', default_opts)
map('n', '<A-j>', ':m .+1<CR>', default_opts)

map('i', '<C-BS>', '<C-w>', default_opts)
map('i', '<C-Delete>', '<C-o>dw', default_opts)

-- Toggle spelling
map('n', '<F10>', ':set spell!<CR>', default_opts)

-- Window controls
map('n', '<A-Up>', '<C-w>k', default_opts)
map('n', '<A-Down>', '<C-w>j', default_opts)
map('n', '<A-Left>', '<C-w>h', default_opts)
map('n', '<A-Right>', '<C-w>l', default_opts)

map('t', '<A-Up>', '<C-\\><C-N><C-w>k', default_opts)
map('t', '<A-Down>', '<C-\\><C-N><C-w>j', default_opts)
map('t', '<A-Left>', '<C-\\><C-N><C-w>h', default_opts)
map('t', '<A-Right>', '<C-\\><C-N><C-w>l', default_opts)

-- Window resizing
map('n', '<S-Up>', ':resize -2<CR>', default_opts)
map('n', '<S-Down>', ':resize +2<CR>', default_opts)
map('n', '<S-Left>', ':vertical resize -2<CR>', default_opts)
map('n', '<S-Right>', ':vertical resize +2<CR>', default_opts)

-- Buffers
map('n', '<TAB>', ':BufferLineCycleNext<CR>', default_opts)
map('n', '<S-TAB>', ':BufferLineCyclePrev<CR>', default_opts)
map('n', '<leader>bn', ':BufferLineMoveNext<CR>', default_opts)
map('n', '<leader>bp', ':BufferLineMovePrev<CR>', default_opts)
map('n', '<leader>z', ':bdelete<CR>', default_opts)
map('n', '<A-z>', ':bdelete!<CR>', default_opts)

map('n', '<leader>e', ":Oil<CR>", default_opts)
map('n', '<A-t>', ':ToggleTerm<CR>', default_opts)

-- Write
map('n', '<leader>w', ':w<CR>', default_opts)
map('n', '<leader>x', ':wqa!<CR>', default_opts)

map('n', '<leader>fc', ':Telescope colorscheme<CR>', default_opts)

-- Sessions
map('n', '<leader>ss', ':SessionSave<CR>', default_opts)
map('n', '<leader>sl', ':SessionLoad<CR>', default_opts)

-- Lsp
map('n', '<leader>lh', '<cmd>lua vim.lsp.buf.hover()<CR>', default_opts)
map('n', '<leader>ld', '<cmd>lua vim.lsp.buf.definition()<CR>', default_opts)
map('n', '<leader>la', '<cmd>lua vim.lsp.buf.code_action()<CR>', default_opts)
map('n', '<leader>lr', '<cmd>lua vim.lsp.buf.rename()<CR>', default_opts)
map('n', '<leader>lI', ':LspInstallInfo<CR>', default_opts)
map('n', '<leader>li', ':LspInfo<CR>', default_opts)
map('n', '<leader>lj', ':Lspsaga diagnostic_jump_next<CR>', default_opts)
map('n', '<leader>lk', ':Lspsaga diagnostic_jump_prev<CR>', default_opts)
map('n', '<leader>lc', ':! sudo make clean install<CR><CR>', default_opts)

map('n', '<leader>lD', ':Telescope lsp_document_diagnostics<CR>', default_opts)
map('n', '<leader>fs', ':Telescope lsp_document_symbols<CR>', default_opts)
map('n', '<leader>fS', ':Telescope lsp_workspace_symbols<CR>', default_opts)
map('n', '<leader><leader>', ':Telescope git_files<CR>', default_opts)
map('n', '<leader>fh', ':Telescope oldfiles<CR>', default_opts)
map('n', '<leader>fw', ':Telescope live_grep<CR>', default_opts)
map('n', '<leader>fb', ':Telescope buffers<CR>', default_opts)
map('n', '<leader>fz', ':Telescope current_buffer_fuzzy_find<CR>', default_opts)

-- Git
map('n', '<leader>gr', ":Gitsigns reset_hunk<CR>", default_opts)
map('n', '<leader>gR', ":Gitsigns reset_buffer<CR>", default_opts)
map('n', '<leader>gs', ":Gitsigns stage_hunk<CR>", default_opts)
map('n', '<leader>gS', ":Gitsigns stage_buffer<CR>", default_opts)
map('n', '<leader>gu', ":Gitsigns undo_stage_hunk<CR>", default_opts)
map('n', '<leader>gl', ":Gitsigns toggle_current_line_blame<CR>", default_opts)
map('n', '<leader>gj', ":Gitsigns next_hunk<CR>", default_opts)
map('n', '<leader>gk', ":Gitsigns prev_hunk<CR>", default_opts)
map('n', '<leader>gg', ":Gitsigns toggle_signs<CR>", default_opts)
map('n', '<leader>gt', ":Telescope git_status<CR>", default_opts)
map('n', '<leader>gc', ":Telescope git_commits<CR>", default_opts)

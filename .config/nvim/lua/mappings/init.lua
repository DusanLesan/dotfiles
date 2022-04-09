local map = vim.api.nvim_set_keymap
local default_opts = {noremap = true, silent = true}

-- Escape by using jk
map('i', 'jk', '<Esc>', {silent = true})

-- Move lines up and down with alt-up/down
map('n', '<A-up>', ':m .-2<CR>', {silent = true})
map('n', '<A-down>', ':m .+1<CR>', {silent = true})

-- Toggle spelling
map('n', '<F10>', ':set spell!<CR>', {silent = true})

-- Window controls
map('n', '<C-h>', '<C-w>h', {silent = true})
map('n', '<C-j>', '<C-w>j', {silent = true})
map('n', '<C-k>', '<C-w>k', {silent = true})
map('n', '<C-l>', '<C-w>l', {silent = true})

-- Window resizing
map('n', '<C-Up>', ':resize -2<CR>', {silent = true})
map('n', '<C-Down>', ':resize +2<CR>', {silent = true})
map('n', '<C-Left>', ':vertical resize -2<CR>', {silent = true})
map('n', '<C-Right>', ':vertical resize +2<CR>', {silent = true})

-- Buffers
map('n', '<TAB>', ':BufferLineCycleNext<CR>', { noremap = true, silent = true })
map('n', '<S-TAB>', ':BufferLineCyclePrev<CR>', { noremap = true, silent = true })
map('n', '<S-x>', ':Bdelete this<CR>', { noremap = true, silent = true })
map('n', '<leader>bn', ':BufferLineMoveNext<CR>', { noremap = true, silent = true })
map('n', '<leader>bp', ':BufferLineMovePrev<CR>', { noremap = true, silent = true })
map('n', '<leader>bc', ':Bdelete this<CR>', { noremap = true, silent = true })

-- FileManagers
map('n', '<leader>e', ':NvimTreeToggle<CR>', default_opts)
map('n', '<leader>E', ':NvimTreeFocus<CR>', default_opts)

-- Write
map('n', '<leader>w', ':w<CR>', default_opts)
map('n', '<leader>x', ':wqa!<CR>', default_opts)

-- Dashboard
map('n', '<leader>d', ':Dashboard<CR>', default_opts)
map('n', '<leader>fh', ':Telescope oldfiles<CR>', default_opts)
map('n', '<leader>ff', ':Telescope find_files<CR>', default_opts)
map('n', '<leader>fc', ':Telescope colorscheme<CR>', default_opts)
map('n', '<leader>n', ':DashboardNewFile<CR>', default_opts)

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
map('n', '<leader>lD', ':Telescope lsp_document_diagnostics<CR>', default_opts)
map('n', '<leader>lj', ':Lspsaga diagnostic_jump_next<CR>', default_opts)
map('n', '<leader>lk', ':Lspsaga diagnostic_jump_prev<CR>', default_opts)
map('n', '<leader>lc', ':! sudo make clean install<CR><CR>', default_opts)

-- Term
map('n', '<leader>ts', ':ToggleTerm size=12 direction=horizontal<CR>', default_opts)
map('n', '<leader>tt', ':ToggleTerm<CR>', default_opts)
map("n", "<leader>tg", "<cmd>lua _lazygit_toggle()<CR>", default_opts)
map("n", "<leader>th", "<cmd>lua _htop_toggle()<CR>", default_opts)
map("n", "<leader>tp", "<cmd>lua _python_toggle()<CR>", default_opts)

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

-- Packer
map('n', '<leader>pi', ":PackerInstall<CR>", default_opts)
map('n', '<leader>pc', ":PackerClean<CR>", default_opts)
map('n', '<leader>ps', ":PackerSync<CR>", default_opts)

-- Comment
local status_ok, commented = pcall(require, "commented")
if status_ok then
commented.setup({
	keybindings = { n = "<C-_>v", v = "<C-_>", nl = "<C-_>" },
	comment_padding = " "
})
end

local map = vim.api.nvim_set_keymap
local default_opts = {noremap = true, silent = true}
local function desc(description)
	return { noremap = true, silent = true, desc = description }
end

map('n', '<A-k>', ':m .-2<CR>', desc('Move line up'))
map('n', '<A-j>', ':m .+1<CR>', desc('Move line down'))

map('i', '<C-BS>', '<C-w>', desc('Delete word left'))
map('i', '<C-Delete>', '<C-o>dw', desc('Delete word right'))

map('n', '<F10>', ':set spell!<CR>', desc('Toggle spell check'))

map('v', '<', '<gv', default_opts)
map('v', '>', '>gv', default_opts)

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

map('n', '<leader>w', ':w<CR>', desc('Save file'))
map('n', '<leader>x', ':wqa!<CR>', desc('Save and quit'))

map('n', '<leader>fc', ':Telescope colorscheme<CR>', default_opts)

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

vim.api.nvim_create_autocmd('FileType', {
	pattern = 'qf',
	callback = function()
		vim.keymap.set('n', '<CR>', '<CR>:cclose<CR>', { buffer = true, silent = true })
	end
})

map('n', '<leader>lc', ':! sudo make clean install<CR><CR>', desc('Execute make clean install'))

map('n', '<leader>fs', ':Telescope lsp_document_symbols<CR>', desc('List symbols'))
map('n', '<leader>fS', ':Telescope lsp_workspace_symbols<CR>', desc('List workspace symbols'))
map('n', '<leader><leader>', ':Telescope git_files<CR>', desc('Find files'))
map('n', '<leader>fh', ':Telescope oldfiles<CR>', desc('List recently opened files'))
map('n', '<leader>fw', ':Telescope live_grep<CR>', desc('Search words'))
map('n', '<leader>fb', ':Telescope buffers<CR>', desc('List buffers'))
map('n', '<leader>fz', ':Telescope current_buffer_fuzzy_find<CR>', desc('Find in buffer'))

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

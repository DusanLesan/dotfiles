local M = {
	'Exafunction/codeium.vim',
	event = 'BufEnter',
	cond = vim.g.vscode == nil
}

function M.config()
	vim.keymap.set('i', '<S-Right>', function () return vim.fn['codeium#Accept']() end, { expr = true, silent = true })
	vim.keymap.set('i', '<S-Up>', function() return vim.fn['codeium#CycleCompletions'](1) end, { expr = true, silent = true })
	vim.keymap.set('i', '<S-Down>', function() return vim.fn['codeium#CycleCompletions'](-1) end, { expr = true, silent = true })
	vim.keymap.set('i', '<S-Left>', function() return vim.fn['codeium#Clear']() end, { expr = true, silent = true })
	vim.g[ "codeium_no_map_tab" ] = 1
end

return M

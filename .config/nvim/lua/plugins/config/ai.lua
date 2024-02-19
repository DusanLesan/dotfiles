local M = {
	'Exafunction/codeium.vim',
	event = 'VeryLazy',
}

function M.config()
	vim.keymap.set('i', '<S-Right>', function () return vim.fn['codeium#Accept']() end, { expr = true, silent = true })
	vim.keymap.set('i', '<S-Up>', function() return vim.fn['codeium#CycleCompletions'](1) end, { expr = true, silent = true })
	vim.keymap.set('i', '<S-Down>', function() return vim.fn['codeium#CycleCompletions'](-1) end, { expr = true, silent = true })
	vim.keymap.set('i', '<S-Left>', function() return vim.fn['codeium#Clear']() end, { expr = true, silent = true })
end

return M

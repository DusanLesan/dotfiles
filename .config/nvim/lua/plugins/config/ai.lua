local M = {
	'github/copilot.vim',
	event = 'BufEnter',
	cond = vim.g.vscode == nil
}

function M.config()
	vim.keymap.set('i', '<A-Left>', function () return vim.fn['copilot#Dismiss']() end, { expr = true, silent = true })
	vim.keymap.set('i', '<A-Down>', function () return vim.fn['copilot#Next']() end, { expr = true, silent = true })
	vim.keymap.set('i', '<A-Up>', function () return vim.fn['copilot#Previous']() end, { expr = true, silent = true })

	vim.keymap.set("i", "<Esc>", function()
		local suggestion = vim.fn["copilot#GetDisplayedSuggestion"]()
		return suggestion.text ~= "" and vim.fn["copilot#Dismiss"]() or vim.api.nvim_replace_termcodes("<Esc>", true, false, true)
	end, { expr = true, silent = true })
end
return M

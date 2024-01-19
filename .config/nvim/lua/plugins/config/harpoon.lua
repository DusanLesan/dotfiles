local M = {
	"ThePrimeagen/harpoon",
	branch = "harpoon2",
	keys = {
		{ "<C-h>", mode = "n" },
		{ "<C-e>", mode = "n" },
		{ "<C-S-P>", mode = "x" },
		{ "<C-S-N>", mode = "n" }
	},
	dependencies = { "nvim-lua/plenary.nvim" }
}

function M.config()
	local harpoon = require("harpoon")
	harpoon:setup()
	vim.keymap.set("n", "<C-h>", function() harpoon:list():append() end)
	vim.keymap.set("n", "<C-e>", function() harpoon.ui:toggle_quick_menu(harpoon:list()) end)
	vim.keymap.set("n", "<C-S-P>", function() harpoon:list():prev() end)
	vim.keymap.set("n", "<C-S-N>", function() harpoon:list():next() end)
end

return M

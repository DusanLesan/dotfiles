local M = {
	"ThePrimeagen/harpoon",
	branch = "harpoon2",
	keys = {
		{ "<C-h>", mode = "n" },
		{ "<C-e>", mode = "n" },
		{ "<leader>n", mode = "n" },
		{ "<leader>p", mode = "n" }
	},
	dependencies = { "nvim-lua/plenary.nvim" }
}

function M.config()
	local harpoon = require("harpoon")
	harpoon:setup()

	local conf = require("telescope.config").values
	local function toggle_telescope(harpoon_files)
		local finder = function()
			local paths = {}
			for _, item in ipairs(harpoon_files.items) do
				table.insert(paths, item.value)
			end

			return require("telescope.finders").new_table({
				results = paths,
			})
		end

		require("telescope.pickers").new({}, {
			prompt_title = "Harpoon",
			finder = finder(),
			previewer = conf.file_previewer({}),
			sorter = conf.generic_sorter({}),
			attach_mappings = function(prompt_bufnr, map)
				map("i", "<C-d>", function()
					local state = require("telescope.actions.state")
					local selected_entry = state.get_selected_entry()
					local current_picker = state.get_current_picker(prompt_bufnr)
					table.remove(harpoon_files.items, selected_entry.index)
					if #harpoon_files.items == 0 then
						require("telescope.actions").close(prompt_bufnr)
					else
						current_picker:refresh(finder())
					end
				end)
				return true
			end,
		}):find()
	end

	vim.keymap.set("n", "<C-e>", function() toggle_telescope(harpoon:list()) end, { desc = "Open harpoon window" })

	vim.keymap.set("n", "<C-h>", function() harpoon:list():add() end)
	vim.keymap.set("n", "<leader>n", function() harpoon:list():prev() end)
	vim.keymap.set("n", "<leader>p", function() harpoon:list():next() end)
end

return M

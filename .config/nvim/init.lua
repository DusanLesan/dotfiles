if not (vim.list and vim.list.unique) then
	vim.list = vim.list or {}
	---@param t string[]
	function vim.list.unique(t)
		local seen = {}
		local j = 1
		for i = 1, #t do
			local v = t[i]
			if not seen[v] then
				seen[v] = true
				t[j] = v
				j = j + 1
			end
		end
		for i = j, #t do
			t[i] = nil
		end
		return t
	end
end

require("core")
require("plugins")
require("mappings")

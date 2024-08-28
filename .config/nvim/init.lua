require("core.init")
if not vim.g.vscode then
	require("plugins")
	require("mappings.init")
end

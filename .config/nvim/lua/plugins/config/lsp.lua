local servers = {
	"clangd",
	"bashls",
	"jdtls",
	"lua_ls",
	"bright_script",
	"pylsp",
	"typos_lsp",
	"jsonls",
}

local M = {
	'neovim/nvim-lspconfig',
	event = "VeryLazy",
	dependencies = {
		{
			'williamboman/mason.nvim',
			cmd = { "Mason", "MasonInstall", "MasonInstallAll", "MasonUpdate" },
			config = true
		},
		{
			'williamboman/mason-lspconfig.nvim',
			config = function()
				require("mason-lspconfig").setup {
					ensure_installed = servers
				}
			end
		}
	}
}

function M.config()
	vim.lsp.enable(servers)

	vim.diagnostic.config({
		virtual_text = true,
		update_in_insert = false,
		underline = true,
		severity_sort = true,
		signs = false
	})
end

return M

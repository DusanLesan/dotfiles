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
			config = function ()
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
		virtual_lines = true,
		underline = true,
		update_in_insert = false,
		severity_sort = true,
		float = {
			border = "rounded",
			source = true,
		},
		signs = {
			text = {
				[vim.diagnostic.severity.ERROR] = "󰅚 ",
				[vim.diagnostic.severity.WARN] = "󰀪 ",
				[vim.diagnostic.severity.INFO] = "󰋽 ",
				[vim.diagnostic.severity.HINT] = "󰌶 ",
			},
			numhl = {
				[vim.diagnostic.severity.ERROR] = "ErrorMsg",
				[vim.diagnostic.severity.WARN] = "WarningMsg",
			},
		},
	})
end

return M

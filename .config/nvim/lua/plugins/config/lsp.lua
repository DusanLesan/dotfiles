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
	vim.o.updatetime = 1500

	local augroup = vim.api.nvim_create_augroup('my.lsp', { clear = true })

	vim.api.nvim_create_autocmd('LspAttach', {
		group = augroup,
		callback = function(args)
			local client = vim.lsp.get_client_by_id(args.data.client_id)
			if not client then return end

			if client:supports_method('textDocument/completion') then
				vim.lsp.completion.enable(true, client.id, args.buf, { autotrigger = true })
			end

			if client.server_capabilities.documentHighlightProvider then
				vim.api.nvim_create_autocmd('CursorHold', {
					group = augroup,
					buffer = args.buf,
					callback = vim.lsp.buf.document_highlight,
				})

				vim.api.nvim_create_autocmd({ 'CursorMoved', 'InsertEnter' }, {
					group = augroup,
					buffer = args.buf,
					callback = vim.lsp.buf.clear_references,
				})
			end
		end
	})

	vim.api.nvim_set_hl(0, "LspReferenceText",  { underline = true })
	vim.api.nvim_set_hl(0, "LspReferenceRead",  { underline = true })
	vim.api.nvim_set_hl(0, "LspReferenceWrite", { underline = true })

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

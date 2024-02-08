local servers = {
	"clangd",
	"bashls",
	"jdtls",
	"lua_ls",
	"bright_script",
	"pylsp"
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

local capabilities = vim.lsp.protocol.make_client_capabilities()
capabilities.textDocument.completion.completionItem = {
	documentationFormat = { 'markdown', 'plaintext' },
	snippetSupport = true,
	preselectSupport = true,
	insertReplaceSupport = true,
	labelDetailsSupport = true,
	deprecatedSupport = true,
	commitCharactersSupport = true,
	tagSupport = { valueSet = { 1 } },
	resolveSupport = {
		properties = {
			'documentation',
			'detail',
			'additionalTextEdits'
		}
	}
}

local config = {
	virtual_text = true,
	update_in_insert = false,
	underline = true,
	severity_sort = true,
	float = {
		focusable = false,
		source = "always",
		header = "",
		prefix = ""
	}
}

vim.diagnostic.config(config)

local function lsp_highlight_document(client)
	-- Set autocommands conditional on server_capabilities
	if client.server_capabilities.document_highlight then
		vim.api.nvim_exec([[
			augroup lsp_document_highlight
			autocmd! * <buffer>
			autocmd CursorHold <buffer> lua vim.lsp.buf.document_highlight()
			autocmd CursorMoved <buffer> lua vim.lsp.buf.clear_references()
			augroup END
		]], false)
	end
end

local function lsp_keymaps(bufnr)
	local opts = { noremap = true, silent = true }
	vim.api.nvim_buf_set_keymap(bufnr, "n", "gD", "<cmd>lua vim.lsp.buf.declaration()<CR>", opts)
	vim.api.nvim_buf_set_keymap(bufnr, "n", "gd", "<cmd>lua vim.lsp.buf.definition()<CR>", opts)
	vim.api.nvim_buf_set_keymap(bufnr, "n", "K", "<cmd>lua vim.lsp.buf.hover()<CR>", opts)
	vim.api.nvim_buf_set_keymap(bufnr, "n", "gi", "<cmd>lua vim.lsp.buf.implementation()<CR>", opts)
	vim.api.nvim_buf_set_keymap(bufnr, "n", "<C-k>", "<cmd>lua vim.lsp.buf.signature_help()<CR>", opts)
	vim.api.nvim_buf_set_keymap(bufnr, "n", "gr", "<cmd>lua vim.lsp.buf.references()<CR>", opts)
	vim.api.nvim_buf_set_keymap(bufnr, "n", "[d", '<cmd>lua vim.diagnostic.goto_prev({ border = "rounded" })<CR>', opts)
	vim.api.nvim_buf_set_keymap(
		bufnr,
		"n",
		"gl",
		'<cmd>lua vim.lsp.diagnostic.show_line_diagnostics({ border = "rounded" })<CR>',
		opts
	)
	vim.api.nvim_buf_set_keymap(bufnr, "n", "]d", '<cmd>lua vim.diagnostic.goto_next({ border = "rounded" })<CR>', opts)
	vim.api.nvim_buf_set_keymap(bufnr, "n", "<leader>q", "<cmd>lua vim.diagnostic.setloclist()<CR>", opts)
	vim.cmd [[ command! Format execute 'lua vim.lsp.buf.formatting()' ]]
end

local function on_attach(client, bufnr)
	lsp_keymaps(bufnr)
	lsp_highlight_document(client)
end

local server_opts = {
	["lua_ls"] = {
		settings = {
			Lua = {
				diagnostics = {
					globals = { "vim" }
				},
				workspace = {
					library = {
						[vim.fn.expand("$VIMRUNTIME/lua")] = true,
						[vim.fn.stdpath("config") .. "/lua"] = true
					}
				}
			}
		}
	}
}

function M.config()
	local lspconfig = require("lspconfig")
	for _, server in ipairs(servers) do
		local opts = {
			on_attach = on_attach,
			capabilities = capabilities
		}

		if server_opts[server] ~= nil then
			opts = vim.tbl_deep_extend("force", server_opts[server], opts)
		end

		lspconfig[server].setup(opts)
	end
	vim.cmd(":LspStart")
end

return M

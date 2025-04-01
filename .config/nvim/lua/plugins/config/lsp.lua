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
	signs = false
}

vim.diagnostic.config(config)

vim.filetype.add({
	extension = {
		brs = 'brs',
	},
})

local function lsp_highlight_document(client)
	vim.o.updatetime = 300

	if client.server_capabilities.documentHighlightProvider then
		local group = vim.api.nvim_create_augroup("lsp_document_highlight", { clear = true })
		vim.api.nvim_create_autocmd("CursorHold", {
			group = group,
			buffer = 0,
			callback = vim.lsp.buf.document_highlight,
		})
		vim.api.nvim_create_autocmd("CursorMoved", {
			group = group,
			buffer = 0,
			callback = vim.lsp.buf.clear_references,
		})
	end
end

local function on_attach(client, _)
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
	},
	["pylsp"] = {
		settings = {
			pylsp = {
				plugins = {
					pycodestyle = {
						maxLineLength = 160,
						ignore = { "W191", "E302", "E305" },
					}
				}
			}
		}
	},
	["clangd"] = {
		root_dir = function(fname)
			return require("lspconfig.util").root_pattern("Makefile")(fname)
				or require("lspconfig.util").root_pattern("compile_commands.json", "compile_flags.txt")(fname)
				or require("lspconfig.util").find_git_ancestor(fname)
		end,
		capabilities = { offsetEncoding = { "utf-16" } },
		init_options = {
			usePlaceholders = true,
			completeUnimported = true,
			clangdFileStatus = true
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
	vim.cmd(":LspStart typos_lsp")
end

return M

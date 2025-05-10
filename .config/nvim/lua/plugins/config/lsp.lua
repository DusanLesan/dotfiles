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

local function get_project_root()
	local util = require("lspconfig.util")
	local fname = vim.api.nvim_buf_get_name(0)
	if fname == "" then return nil end
	return util.root_pattern("Makefile")(fname)
		or util.root_pattern("compile_commands.json", "compile_flags.txt")(fname)
		or util.find_git_ancestor(fname)
end

local function get_bookmark_file()
	local root = get_project_root()
	if not root then
		vim.notify("Could not determine project root.", vim.log.levels.ERROR)
		return nil
	end

	local hash = vim.fn.sha256(root)
	local bookmark_file = vim.fn.stdpath("data") .. "/bookmarks/" .. hash .. ".bookmarks"
	vim.fn.mkdir(vim.fn.fnamemodify(bookmark_file, ":h"), "p")
	return bookmark_file
end

vim.api.nvim_create_user_command("OpenProjectBookmarks", function()
	local bookmark_file = get_bookmark_file()
	if not bookmark_file then return end
	vim.cmd("edit " .. vim.fn.fnameescape(bookmark_file))
end, {})

vim.api.nvim_create_user_command("OpenProjectBookmarks", function()
	local root = get_project_root()
	if not root then
		vim.notify("Could not determine project root.", vim.log.levels.ERROR)
		return
	end

	local hash = vim.fn.sha256(root)
	local path = vim.fn.stdpath("data") .. "/bookmarks/" .. hash .. ".bookmarks"

	vim.fn.mkdir(vim.fn.fnamemodify(path, ":h"), "p")
	vim.cmd("edit " .. vim.fn.fnameescape(path))
end, {})

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
	},
	["bright_script"] = {
		settings = {
			brightscript = {
				diagnostics = {
					enabled = true,
				}
			}
		},
		flags = {
			debounce_text_changes = 50,
			allow_incremental_sync = false,
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

	vim.keymap.set("n", "<leader>B", "<Cmd>OpenProjectBookmarks<CR>", { desc = "Open project bookmarks" })
	vim.keymap.set("n", "<leader>b", function()
		local bookmark_file = get_bookmark_file()
		if not bookmark_file then return end
		local line_content = vim.fn.getline(".")
		local filepath = vim.fn.expand("%:p")
		line_content = line_content:gsub("'", "\\'")
		local entry = string.format("'%s' %s\n", line_content, filepath)
		local f = io.open(bookmark_file, "a")
		if not f then return end
		f:write(entry)
		f:close()
		vim.notify("Added to bookmarks: " .. entry)
	end, { desc = "Append current line content and file path to project bookmarks" })
end

return M

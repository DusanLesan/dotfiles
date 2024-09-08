local M = {
	'hrsh7th/nvim-cmp',
	event = "InsertEnter",
	dependencies = {
		"hrsh7th/cmp-nvim-lsp",
		"hrsh7th/cmp-nvim-lua",
		"hrsh7th/cmp-buffer",
		"hrsh7th/cmp-path",
		"hrsh7th/vim-vsnip",
		"hrsh7th/cmp-vsnip",
		"hrsh7th/vim-vsnip-integ"
	}
}

vim.opt.completeopt = "menuone,noselect"
vim.g.vsnip_snippet_dir = os.getenv('HOME') .. '/.config/nvim/snippets/'

local feedkey = function(key, mode)
	vim.api.nvim_feedkeys(vim.api.nvim_replace_termcodes(key, true, true, true), mode, true)
end

function M.config()
	local cmp = require("cmp")
	cmp.setup {
		snippet = {
			expand = function(args)
				-- require("luasnip").lsp_expand(args.body)
				vim.fn["vsnip#anonymous"](args.body) -- For `vsnip` users.
			end
		},
		formatting = {
			format = function(entry, vim_item)
				-- load lspkind icons
				vim_item.kind = string.format(
					"%s",
					vim_item.kind
				)

				vim_item.menu = ({
					nvim_lsp = "[LSP]",
					nvim_lua = "[Lua]",
					buffer = "[BUF]"
				})[entry.source.name]

				return vim_item
			end
		},
		mapping = {
			["<C-p>"] = cmp.mapping.select_prev_item(),
			["<C-n>"] = cmp.mapping.select_next_item(),
			["<C-d>"] = cmp.mapping.scroll_docs(-4),
			["<C-f>"] = cmp.mapping.scroll_docs(4),
			["<C-Space>"] = cmp.mapping.complete(),
			["<Esc>"] = cmp.mapping.close(),
			["<CR>"] = cmp.mapping.confirm {
				behavior = cmp.ConfirmBehavior.Replace,
				select = true
			},
			["<Tab>"] = cmp.mapping(function(fallback)
				if cmp.visible() then
					cmp.select_next_item()
				else
					fallback() -- The fallback function sends a already mapped key. In this case, it's probably `<Tab>`.
				end
			end, {"i", "s"}),
			["<S-Tab>"] = cmp.mapping(function()
				if cmp.visible() then
					cmp.select_prev_item()
				elseif vim.fn["vsnip#jumpable"](-1) == 1 then
					feedkey("<Plug>(vsnip-jump-prev)", "")
				end
			end, {"i", "s"})
		},
		sources = {
			{ name = "nvim_lsp" },
			{ name = "vsnip" },
			{ name = "buffer" },
			{ name = "nvim_lua" },
			{ name = "path" }
		}
	}
end

return M

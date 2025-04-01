local M = {
	"rest-nvim/rest.nvim",
	ft = "http",
	dependencies = {
		"nvim-treesitter/nvim-treesitter",
	}
}

function M.config()
	local rest = require("rest-nvim")
	rest.setup {
		encode_url = true,
		result = {
			show_url = false,
			show_curl_command = false,
			show_http_info = false,
			show_headers = false,
			formatters = {
				json = "jq"
			},
		}
	}
end

return M

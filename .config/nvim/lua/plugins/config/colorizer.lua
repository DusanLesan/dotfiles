local M = {
	"catgoose/nvim-colorizer.lua",
	event = "VeryLazy"
}

local opts = {
	user_default_options = {
		RGB = true,
		RRGGBB = true,
		names = false,
		RRGGBBAA = true,
		rgb_fn = true,
		hsl_fn = true,
		css = true,
		css_fn = true
	}
}

function M.config()
	local colorizer = require("colorizer")
	colorizer.setup(opts)
	colorizer.attach_to_buffer(0)
end

return M

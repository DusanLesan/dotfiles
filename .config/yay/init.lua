local DELAY_DAYS = tonumber(os.getenv("DELAY_DAYS")) or 3

yay.create_autocmd("UpgradeSelect", {
	desc = "skip AUR upgrades younger than " .. DELAY_DAYS .. " days",
	callback = function(event)
		local exclude = {}
		local cutoff = os.time() - (DELAY_DAYS * 24 * 60 * 60)

		for _, pkg in ipairs(event.data.upgrades) do
			if pkg.repository == "aur" and pkg.last_modified >= cutoff then
				local age_days = math.floor((os.time() - pkg.last_modified) / 86400)
				print(string.format(
					"Holding %s (modified %d day(s) ago, waiting for %d)",
					pkg.name, age_days, DELAY_DAYS
				))
				table.insert(exclude, pkg.name)
			end
		end

		return { exclude = exclude, skip_menu = false }
	end,
})

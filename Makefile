
all: copy

copy:
	mkdir -p ./bin
	cp ~/bin/57 ./bin
	cp ~/bin/autostart ./bin
	cp ~/bin/autostart_dwm ./bin
	cp ~/bin/clear_cache ./bin
	cp ~/bin/communication ./bin
	cp ~/bin/display_switch ./bin
	cp ~/bin/limit_ap ./bin
	cp ~/bin/location_list ./bin
	cp ~/bin/locations ./bin
	cp ~/bin/pass_prompt ./bin
	cp ~/bin/sharevlc ./bin
	cp ~/bin/start_desktop ./bin
	cp ~/bin/start_polybar ./bin
	cp ~/bin/white ./bin
	
	cp -r ~/.config/polybar .
	cp ~/.zshrc .
	cp ~/.bashrc .

install: all

#!/bin/sh

eval $(gnome-keyring-daemon --start) > /dev/null
ln -sf "$SSH_AUTH_SOCK" /tmp/ssh_auth_sock

xrdb ${XDG_CONFIG_HOME:-$HOME/.config}/x11/xresources &
autostart &

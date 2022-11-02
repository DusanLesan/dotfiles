#!/bin/sh

export $(gnome-keyring-daemon --start)
xrdb ${XDG_CONFIG_HOME:-$HOME/.config}/x11/xresources &
autostart &

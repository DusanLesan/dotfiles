#!/bin/sh

xrdb ${XDG_CONFIG_HOME:-$HOME/.config}/x11/xresources &
autostart &

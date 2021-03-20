#!/bin/zsh

export ROKU_DEVICE_ID="3a25248a-6a9c-5ece-8c4f-3be26c3c1ab5,9db93e76-fcdb-5dea-b029-1ae94e850c71"

# Adds `~/.local/bin` and its subdirectories to $PATH
export PATH="$PATH:$(du "$HOME/.local/bin" | cut -f2 | paste -sd ':')"

# Default programs:
export EDITOR="nvim"
export TERMINAL="alacritty"
export BROWSER="brave"

# ~/ Clean-up:
export XDG_CONFIG_HOME="$HOME/.config"
export XDG_DATA_HOME="$HOME/.local/share"
export XDG_CACHE_HOME="$HOME/.cache"
export NOTMUCH_CONFIG="${XDG_CONFIG_HOME:-$HOME/.config}/notmuch-config"
export GTK2_RC_FILES="${XDG_CONFIG_HOME:-$HOME/.config}/gtk-2.0/gtkrc-2.0"
export INPUTRC="${XDG_CONFIG_HOME:-$HOME/.config}/inputrc"
export ZDOTDIR="${XDG_CONFIG_HOME:-$HOME/.config}/zsh"
export TMUX_TMPDIR="$XDG_RUNTIME_DIR"
export ANDROID_SDK_HOME="${XDG_CONFIG_HOME:-$HOME/.config}/android"
export HISTFILE="${XDG_DATA_HOME:-$HOME/.local/share}/history"

# Other program settings:
export DICS="/usr/share/stardict/dic/"
export SUDO_ASKPASS="$HOME/.local/bin/askpass"
export QT_QPA_PLATFORMTHEME="gtk2"	# Have QT use gtk2 theme.

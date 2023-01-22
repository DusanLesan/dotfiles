#!/bin/sh
# Adds `~/.local/bin` and its subdirectories to $PATH
export PATH="$PATH:$(du "$HOME/.local/bin" | cut -f2 | paste -sd ':')"

# Default programs:
export EDITOR="nvim"
export TERMINAL="alacritty -e"
export BROWSER="brave"

# ~/ Clean-up:
export XDG_CONFIG_HOME="$HOME/.config"
export XDG_DATA_HOME="$HOME/.local/share"
export XDG_STATE_HOME="$HOME/.local/share"
export XDG_CACHE_HOME="$HOME/.cache"
export GNUPGHOME="$XDG_DATA_HOME/gnupg"
export GTK2_RC_FILES="$XDG_CONFIG_HOME/gtk-2.0/gtkrc-2.0"
export ZDOTDIR="$XDG_CONFIG_HOME/zsh"
export LESSHISTFILE="-"
export VSCODE_PORTABLE="$XDG_CONFIG_HOME/code-oss"
export WGETRC="$XDG_CONFIG_HOME/wget/wgetrc"

# Other program settings:
export SSH_AUTH_SOCK=/tmp/ssh_auth_sock
export SUDO_ASKPASS="$HOME/.local/bin/askpass"
export QT_QPA_PLATFORMTHEME="gtk2"	# Have QT use gtk2 theme.

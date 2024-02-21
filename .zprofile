#!/bin/sh

# Adds `~/.local/bin` and its subdirectories to $PATH
export PATH="$(du "$HOME/.local/bin" | cut -f2 | paste -sd ':'):$PATH"

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
export NPM_CONFIG_USERCONFIG=$XDG_CONFIG_HOME/npm/npmrc
export CUDA_CACHE_PATH="$XDG_CACHE_HOME"/nv

# Other program settings:
export SSH_AUTH_SOCK=$XDG_RUNTIME_DIR/gcr/ssh
export SUDO_ASKPASS="$HOME/.local/bin/askpass"
export QT_QPA_PLATFORMTHEME="gtk2"	# Have QT use gtk2 theme.

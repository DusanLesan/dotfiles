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
export XDG_CACHE_HOME="$HOME/.cache"
export MBSYNCRC="${XDG_CONFIG_HOME:-$HOME/.config}/mbsync/mbsyncrc"
export NOTMUCH_CONFIG="${XDG_CONFIG_HOME:-$HOME/.config}/notmuch-config"
export GNUPGHOME="${XDG_DATA_HOME:-$HOME/.local/share}/gnupg"
export PASSWORD_STORE_DIR="${XDG_DATA_HOME:-$HOME/.local/share}/password-store"
export GTK2_RC_FILES="${XDG_CONFIG_HOME:-$HOME/.config}/gtk-2.0/gtkrc-2.0"
export INPUTRC="${XDG_CONFIG_HOME:-$HOME/.config}/inputrc"
export ZDOTDIR="${XDG_CONFIG_HOME:-$HOME/.config}/zsh"
export TMUX_TMPDIR="$XDG_RUNTIME_DIR"
export LESSHISTFILE="-"
export _JAVA_OPTIONS=-Djava.util.prefs.userRoot="$XDG_CONFIG_HOME/java"
export ANDROID_SDK_HOME="${XDG_CONFIG_HOME:-$HOME/.config}/android"
export HISTFILE="${XDG_DATA_HOME:-$HOME/.local/share}/history"
export VSCODE_PORTABLE="$XDG_CONFIG_HOME/code-oss"
export SSB_HOME="$XDG_DATA_HOME/zoom"

# Other program settings:
export SSH_AUTH_SOCK=/tmp/ssh_auth_sock
export SUDO_ASKPASS="$HOME/.local/bin/askpass"
export QT_QPA_PLATFORMTHEME="gtk2"	# Have QT use gtk2 theme.

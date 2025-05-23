function lfcd() {
	tmp="$(mktemp -uq)"
	trap 'rm -f $tmp >/dev/null 2>&1' HUP INT QUIT TERM PWR EXIT
	lfrun -last-dir-path="$tmp" "$*"
	if [ -f "$tmp" ]; then
		dir="$(cat "$tmp")"
		[ -d "$dir" ] && [ "$dir" != "$(pwd)" ] && cd "$dir"
	fi
}

if [ "$_START_LFCD" ]; then
	[ -e "$_START_LFCD" ] && lfcd "$_START_LFCD" || lfcd
fi

## Options section
setopt correct                                                   # Auto correct mistakes
setopt extendedglob                                              # Extended globbing. Allows using regular expressions with *
setopt nocaseglob                                                # Case insensitive globbing
setopt rcexpandparam                                             # Array expansion with parameters
setopt nocheckjobs                                               # Don't warn about running processes when exiting
setopt numericglobsort                                           # Sort filenames numerically when it makes sense
setopt nobeep                                                    # No beep
setopt appendhistory                                             # Immediately append history instead of overwriting
setopt histignorealldups                                         # If a new command is a duplicate, remove the older one
setopt autocd                                                    # If only directory path is entered, cd there.
setopt HIST_IGNORE_SPACE                                         # Remove command lines starting with spacce from the history
autoload -U colors && colors                                     # Load colors
stty stop undef                                                  # Disable ctrl-s to freeze terminal.
autoload -Uz vcs_info                                            # Load git info
precmd() { vcs_info }
autoload -U compinit && compinit

zstyle ':completion:*' matcher-list 'm:{a-zA-Z}={A-Za-z}'        # Case insensitive tab completion
zstyle ':completion:*' list-colors "${(s.:.)LS_COLORS}"          # Colored completion (different colors for dirs/files/etc)
zstyle ':completion:*' rehash true                               # Automatically find new executables in path
zstyle ':vcs_info:git*' formats "%{$fg[yellow]%}[%b]"            # Format git info
# Speed up completions
zstyle ':completion:*' accept-exact '*(N)'
zstyle ':completion:*' use-cache on
zstyle ':completion:*' cache-path ~/.zsh/cache

# History in cache directory:
HISTSIZE=100000
SAVEHIST=100000
HISTFILE=~/.config/zsh/zhistory

WORDCHARS=${WORDCHARS//\/[&.;]}                                  # Don't consider certain characters part of the word

## Keybindings section
# Edit line in vim with ctrl-e:
autoload edit-command-line; zle -N edit-command-line
bindkey '^e' edit-command-line

bindkey '^[[7~' beginning-of-line                                # Home key
bindkey '^[[H' beginning-of-line                                 # Home key
if [[ "${terminfo[khome]}" != "" ]]; then
    bindkey "${terminfo[khome]}" beginning-of-line               # [Home] - Go to beginning of line
fi
bindkey '^[[8~' end-of-line                                      # End key
bindkey '^[[F' end-of-line                                       # End key
if [[ "${terminfo[kend]}" != "" ]]; then
    bindkey "${terminfo[kend]}" end-of-line                      # [End] - Go to end of line
fi
bindkey '^[[2~' overwrite-mode                                   # Insert key
bindkey '^[[3~' delete-char                                      # Delete key
bindkey '^[[C' forward-char                                      # Right key
bindkey '^[[D' backward-char                                     # Left key
bindkey '^[[5~' history-beginning-search-backward                # Page up key
bindkey '^[[6~' history-beginning-search-forward                 # Page down key

# Navigate words with ctrl+arrow keys
bindkey '^[Oc' forward-word                                      #
bindkey '^[Od' backward-word                                     #
bindkey '^[[1;5D' backward-word                                  #
bindkey '^[[1;5C' forward-word                                   #
bindkey '^H' backward-kill-word                                  # Delete previous word with ctrl+backspace
bindkey '^[[Z' undo                                              # Shift+tab undo last action
bindkey -s '^o' '^ulfcd\n'

# Load aliases
source "${XDG_CONFIG_HOME:-$HOME/.config}/aliasrc"

# Enable substitution for prompt
setopt prompt_subst

[ -z "$SSH_CLIENT" ] && promptColor="white" || promptColor="red"
PS1="%{$fg[$promptColor]%}%c $ "
RPROMPT=$'$vcs_info_msg_0_'

## Vim mode config
# Activate vim mode
bindkey -v '^?' backward-delete-char

# Remove mode switching delay.
KEYTIMEOUT=1

# Change cursor shape for different vi modes.
function zle-keymap-select () {
	case $KEYMAP in
		vicmd) echo -ne '\e[1 q';;      # block
		viins|main) echo -ne '\e[5 q';; # beam
	esac
}
zle -N zle-keymap-select

zle-line-init() {
	zle -K viins # initiate `vi insert` as keymap (can be removed if `bindkey -V` has been set elsewhere)
	echo -ne "\e[5 q"
}
zle -N zle-line-init

# ci", ci', ci`, di", etc
autoload -U select-quoted
zle -N select-quoted
for m in visual viopp; do
	for c in {a,i}{\',\",\`}; do
		bindkey -M $m $c select-quoted
	done
done

# ci{, ci(, ci<, di{, etc
autoload -U select-bracketed
zle -N select-bracketed
for m in visual viopp; do
	for c in {a,i}${(s..)^:-'()[]{}<>bB'}; do
		bindkey -M $m $c select-bracketed
	done
done

echo -ne '\e[5 q' # Use beam shape cursor on startup.
preexec() { echo -ne '\e[5 q' ;} # Use beam shape cursor for each new prompt.

## FZF
export FZF_COMPLETION_TRIGGER='aa'
source /usr/share/fzf/completion.zsh
source /usr/share/fzf/key-bindings.zsh

## Plugins section
# Use syntax highlighting
source /usr/share/zsh/plugins/zsh-syntax-highlighting/zsh-syntax-highlighting.zsh
# Use history substring search
source /usr/share/zsh/plugins/zsh-history-substring-search/zsh-history-substring-search.zsh
# bind UP and DOWN arrow keys to history substring search
zmodload zsh/terminfo
bindkey "$terminfo[kcuu1]" history-substring-search-up
bindkey "$terminfo[kcud1]" history-substring-search-down
bindkey '^[[A' history-substring-search-up
bindkey '^[[B' history-substring-search-down

source /usr/share/zsh/plugins/zsh-autosuggestions/zsh-autosuggestions.zsh
ZSH_AUTOSUGGEST_BUFFER_MAX_SIZE=20
ZSH_AUTOSUGGEST_HIGHLIGHT_STYLE='fg=8'

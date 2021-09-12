## Options section
setopt correct                                                   # Auto correct mistakes
setopt extendedglob                                              # Extended globbing. Allows using regular expressions with *
setopt nocaseglob                                                # Case insensitive globbing
setopt rcexpandparam                                             # Array expension with parameters
setopt nocheckjobs                                               # Don't warn about running processes when exiting
setopt numericglobsort                                           # Sort filenames numerically when it makes sense
setopt nobeep                                                    # No beep
setopt appendhistory                                             # Immediately append history instead of overwriting
setopt histignorealldups                                         # If a new command is a duplicate, remove the older one
setopt autocd                                                    # If only directory path is entered, cd there.
autoload -U colors && colors                                     # Load colors
autoload -U compinit && compinit -d
stty stop undef                                                  # Disable ctrl-s to freeze terminal.
autoload -Uz vcs_info                                            # Load git info
precmd() { vcs_info }

zstyle ':completion:*' matcher-list 'm:{a-zA-Z}={A-Za-z}'        # Case insensitive tab completion
zstyle ':completion:*' list-colors "${(s.:.)LS_COLORS}"          # Colored completion (different colors for dirs/files/etc)
zstyle ':completion:*' rehash true                               # Automatically find new executables in path
zstyle ':vcs_info:git*' formats "%{$fg[yellow]%}[%b]"            # Format git info
# Speed up completions
zstyle ':completion:*' accept-exact '*(N)'
zstyle ':completion:*' use-cache on
zstyle ':completion:*' cache-path ~/.zsh/cache

# History in cache directory:
HISTSIZE=10000
SAVEHIST=10000
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

# Load aliases
source "${XDG_CONFIG_HOME:-$HOME/.config}/aliasrc"

# Enable substitution for prompt
setopt prompt_subst

[ -z "$SSH_CLIENT" ] && promptColor="white" || promptColor="red"
PS1="%{$fg[$promptColor]%}%c $ "
RPROMPT=$'$vcs_info_msg_0_'

# Color man pages
export LESS_TERMCAP_mb=$'\E[01;32m'
export LESS_TERMCAP_md=$'\E[01;32m'
export LESS_TERMCAP_me=$'\E[0m'
export LESS_TERMCAP_se=$'\E[0m'
export LESS_TERMCAP_so=$'\E[01;47;34m'
export LESS_TERMCAP_ue=$'\E[0m'
export LESS_TERMCAP_us=$'\E[01;36m'
export LESS=-r

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

## Vim mode config
# Activate vim mode
bindkey -v '^?' backward-delete-char

# Remove mode switching delay.
KEYTIMEOUT=1

# Change cursor shape for different vi modes.
function zle-keymap-select {
    if [[ ${KEYMAP} == vicmd ]] ||
        [[ $1 = 'block' ]];
    then
        echo -ne '\e[1 q'
    elif [[ ${KEYMAP} == main ]] ||
        [[ ${KEYMAP} == viins ]] ||
        [[ ${KEYMAP} = '' ]] ||
        [[ $1 = 'beam' ]];
    then
        echo -ne '\e[5 q'
    fi
}
zle -N zle-keymap-select

# Use beam shape cursor on startup.
echo -ne '\e[5 q'

# Use beam shape cursor for each new prompt.
preexec() {
    echo -ne '\e[5 q'
}

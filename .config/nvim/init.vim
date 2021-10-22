call plug#begin(system('echo -n "${XDG_CONFIG_HOME:-$HOME/.config}/nvim/plugged"'))
Plug 'dusanlesan/vimbrs'
Plug 'neovim/nvim-lspconfig'
Plug 'tomasiser/vim-code-dark'
Plug 'nvim-treesitter/nvim-treesitter', {'do': ':TSUpdate'}
Plug 'hrsh7th/nvim-cmp' |
	\ Plug 'hrsh7th/cmp-buffer' |
	\ Plug 'hrsh7th/cmp-path' |
	\ Plug 'hrsh7th/cmp-nvim-lsp'
Plug 'preservim/nerdtree' |
	\ Plug 'Xuyuanp/nerdtree-git-plugin'
Plug 'nvim-telescope/telescope.nvim' |
	\ Plug 'nvim-lua/plenary.nvim'
call plug#end()

:lua require('cmp-config')
:lua require('treesitter')

" Use system clipboard
set clipboard+=unnamedplus

set title
set mouse=a
set ignorecase
set smartcase
set encoding=utf-8
set number relativenumber
set list
set whichwrap+=<,>
colorscheme codedark
syntax on

" Tab Settings
set shiftwidth=2
set softtabstop=2
set tabstop=2

" Autocompletion
set wildmode=longest,list,full

" Fix splitting
set splitbelow splitright

" Don't fold by default
set nofoldenable

" Disable X mode
nnoremap Q <nop>

" Escape by using ii or aa
inoremap ii <esc>
inoremap aa <esc>

" Move lines up and down with alt-up/down
nnoremap <A-up> :m .-2<CR>==
nnoremap <A-down> :m .+1<CR>==
vnoremap <A-up> :m '<-2<CR>gv=gv
vnoremap <A-down> :m '>+1<CR>gv=gv
inoremap <A-up> <Esc>:m .-2<CR>==gi
inoremap <A-down> <Esc>:m .+1<CR>==gi

" Toggle spelling
noremap <silent><F10> :set spell!<CR>

" Scroll with space and alt-space
nnoremap <Space> <C-D>
nnoremap <A-Space> <C-U>

" Nerdtree
let NERDTreeShowHidden=1
nnoremap <leader>n :NERDTreeFocus<CR>
nnoremap <leader>fd <cmd>Telescope file_browser hidden=true<cr>
nnoremap <leader>ff <cmd>Telescope find_files hidden=true<cr>
nnoremap <leader>fg <cmd>Telescope live_grep<cr>
nnoremap <leader>fb <cmd>Telescope buffers<cr>
nnoremap <leader>fh <cmd>Telescope help_tags<cr>


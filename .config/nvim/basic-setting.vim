" Use system clipboard
set clipboard+=unnamedplus

set mouse=a
syntax on
set ignorecase
set smartcase
set encoding=utf-8
set number relativenumber
set list
set whichwrap+=<,>

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


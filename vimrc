set laststatus=2
colorscheme 256-grayvim
let mapleader = " "
noremap <silent> <C-S>          :update<CR>
vnoremap <silent> <C-S>         <C-C>:update<CR>
inoremap <silent> <C-S>         <C-O>:update<CR>
"noremap <silent> <Leader-S>          :update<CR>
"vnoremap <silent> <Leader-S>         <C-C>:update<CR>
"noremap <silent> <Leader-S>         <C-O>:update<CR>
noremap <Leader>s :update<CR>
"""" FROM DOUG BLACKS VIMRC
 " Colors {{{
 syntax enable           " enable syntax processing
 "colorscheme badwolf
 " }}}
 " Misc {{{
 set ttyfast                     " faster redraw
 set backspace=indent,eol,start
 " }}}
 " Spaces & Tabs {{{
 set tabstop=4           " 4 space tab
 set expandtab           " use spaces for tabs
 set softtabstop=4       " 4 space tab
 set shiftwidth=4
 set modelines=1
 filetype indent on
 filetype plugin on
 set autoindent
 " }}}
 " UI Layout {{{
 set number              " show line numbers
 set showcmd             " show command in bottom bar
 set nocursorline          " highlight current line
 set wildmenu
 " set lazyredraww
 set showmatch           " higlight matching parenthesis
 " }}}
 " Searching {{{
 set ignorecase          " ignore case when searching
 set incsearch           " search as characters are entered
 set hlsearch            " highlight all matches
 " }}}
 " Folding {{{
 "=== folding ===
 set foldmethod=indent   " fold based on indent level
 set foldnestmax=10      " max 10 depth
 set foldenable          " don't fold files by default on open
 nnoremap <space> za
 set foldlevelstart=10    " start with fold level of 1
 " }}}




"NeoBundle Scripts-----------------------------
if &compatible
  set nocompatible               " Be iMproved
  endif
"
  " Required:
  set runtimepath^=/home/accounts/studenti/id305svp/.vim/bundle/neobundle.vim/
"
  " Required:
  call neobundle#begin(expand('/home/accounts/studenti/id305svp/.vim/bundle'))
"
  " Let NeoBundle manage NeoBundle
  " Required:
  NeoBundleFetch 'Shougo/neobundle.vim'
"
"  " Add or remove your Bundles here:
  NeoBundle 'Shougo/neosnippet.vim'
  NeoBundle 'Shougo/neosnippet-snippets'
  NeoBundle 'tpope/vim-fugitive'
  NeoBundle 'ctrlpvim/ctrlp.vim'
  NeoBundle 'flazz/vim-colorschemes'
"
"  " You can specify revision/branch/tag.
"  NeoBundle 'Shougo/vimshell', { 'rev' : '3787e5' }
  NeoBundle 'sjl/badwolf'
  NeoBundle 'scrooloose/nerdtree'
  NeoBundle 'Lokaltog/powerline', {'rtp': 'powerline/bindings/vim/'}
"
  " Required:
  call neobundle#end()
"
  " Required:
  filetype plugin indent on
"
"  " If there are uninstalled bundles found on startup,
"  " this will conveniently prompt you to install them.
  NeoBundleCheck
"  "End NeoBundle Scripts-------------------------
"
" Stick this in your vimrc to open NERDTree with Ctrl+n
map <C-n> :NERDTreeToggle<CR>
set clipboard=unnamed

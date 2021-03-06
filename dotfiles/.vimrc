"set nocompatible
"filetype off
"
"set rtp+=~/.vim/bundle/Vundle.vim
"call vundle#begin()
"Plugin 'VundleVim/Vundle.vim'
"Plugin 'ycm-core/YouCompleteMe'
"call vundle#end()
filetype plugin indent on

function PrintHighlighter()
  echo "hi<" . synIDattr(synID(line("."),col("."),1),"name") . '> trans<' . synIDattr(synID(line("."),col("."),0),"name") . "> lo<" . synIDattr(synIDtrans(synID(line("."),col("."),1)),"name") . ">"
endfunction

au BufReadPost * if line("'\"") > 1 && line("'\"") <= line("$") | exe "normal! g'\"" | endif

syntax on
set nonumber
set backspace=indent,eol,start
set smartcase
set ignorecase
set smartindent
set cinoptions=(0
set fillchars+=stl:-
set fillchars+=stlnc:-
set hlsearch
set formatoptions+=cro
set wildmode=full
set t_Co=16
set notermguicolors
set noswapfile
set background=dark
set textwidth=80
"set sts=2 ts=2 sw=2 expandtab
set sts=4 ts=4 sw=4 expandtab
"set sts=8 ts=8 sw=8 noexpandtab

au BufRead,BufNewFile,BufEnter */linux/*,i2c-ast.c :set sw=8 ts=8 sts=8 noexpandtab

augroup ft_c
  autocmd!
  autocmd Syntax c syn keyword Statement let loop
  "autocmd Syntax c syn match dFunction "\zs\(\k\w*\)*\ze([^)]*)\s*{"
  "autocmd Syntax c syn match dFunction "\zs\(\k\w*\)*\ze([^)]*)\s*$"
  autocmd Syntax c syn keyword Type uint8_t uint16_t uint32_t uint64_t
  autocmd Syntax c syn keyword Type int8_t int16_t int32_t int64_t
  autocmd Syntax c syn keyword Type i8 i16 i32 i64 u8 u16 u32 u64
augroup end

"augroup ft_cpp
"  autocmd!
"  autocmd Syntax cpp syn keyword Statement let loop
"  "autocmd Syntax cpp syn match dFunction "\zs\(\k\w*\)*\s*\ze("
"  autocmd Syntax cpp syn keyword Type uint8_t uint16_t uint32_t uint64_t
"  autocmd Syntax cpp syn keyword Type int8_t int16_t int32_t int64_t
"  autocmd Syntax cpp syn keyword Type i8 i16 i32 i64 u8 u16 u32 u64
"augroup end

hi StatusLine     ctermfg=0 ctermbg=7
hi StatusLineNC   ctermfg=0 ctermbg=7
hi VertSplit      ctermfg=0 ctermbg=7

" Visual Studio Code
"hi Comment        ctermfg=8
"hi NonText        ctermfg=0
"hi MatchParen     ctermfg=11 ctermbg=0
"hi Visual         ctermfg=0
"hi Todo           ctermfg=8 ctermbg=0
"hi LineNr         ctermfg=7
"hi Statement      ctermfg=5
"hi Include        ctermfg=5
"hi String         ctermfg=3
"hi Comment        ctermfg=2
"hi PreProc        ctermfg=4
"hi Number         ctermfg=10
"hi Type           ctermfg=4
"hi dFunction      ctermfg=11
"hi Special        ctermfg=3
"hi Todo           ctermfg=2
"hi cParenError    ctermfg=7
"hi cErrInBracket  ctermfg=7
"hi rustOperator   ctermfg=7
"hi SignColumn     ctermbg=0
"hi Pmenu          ctermfg=0 ctermbg=7
"hi PemnuSel       ctermfg=0

"set timeoutlen=0
"set updatetime=100

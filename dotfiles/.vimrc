function PrintHighlighter()
  echo "hi<" . synIDattr(synID(line("."),col("."),1),"name") . '> trans<' . synIDattr(synID(line("."),col("."),0),"name") . "> lo<" . synIDattr(synIDtrans(synID(line("."),col("."),1)),"name") . ">"
endfunction

if has("autocmd")
  au BufReadPost * if line("'\"") > 1 && line("'\"") <= line("$") | exe "normal! g'\"" | endif
endif

filetype plugin indent on
syntax on
set smartcase
set ignorecase
set smartindent
set cinoptions=(0
set sts=2 ts=8 sw=2 expandtab
set fillchars+=stl:-
set fillchars+=stlnc:-
set hlsearch
set formatoptions+=cro
set wildmode=full
set t_Co=16
set notermguicolors

"colorscheme solarized
"set background=dark
"hi StatusLine       ctermfg=8  ctermbg=12
"hi StatusLineNC     ctermfg=8  ctermbg=12
"hi VertSplit        ctermfg=12 ctermbg=8
"hi cppSTLNamespace  ctermfg=12

" augroup ft_cpp
"   autocmd!
"   autocmd Syntax cpp syn match cFunctionName "[a-zA-Z_][a-zA-Z_0-9]*("me=e-1 contained
"   autocmd Syntax cpp syn match cFunction "[a-zA-Z_][a-zA-Z_0-9]*(.*{$" contains=cFunctionName,cStorageClass,cType
"   autocmd Syntax cpp hi cFunctionName ctermfg=5
" augroup end

hi StatusLine     ctermfg=0 ctermbg=7
hi StatusLineNC   ctermfg=0 ctermbg=7
hi VertSplit      ctermfg=0 ctermbg=7
hi NonText        ctermfg=0
hi Visual         ctermfg=0 ctermbg=3
hi Search         ctermfg=0 ctermbg=3

hi Type ctermfg=7
hi Constant ctermfg=7
hi Special ctermfg=7

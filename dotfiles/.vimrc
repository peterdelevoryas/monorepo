set smartcase
set ignorecase
set smartindent
set cinoptions=(0
set sts=2 ts=8 sw=2 expandtab

if has("autocmd")
  au BufReadPost * if line("'\"") > 1 && line("'\"") <= line("$") | exe "normal! g'\"" | endif
endif

set fillchars+=stl:-
set fillchars+=stlnc:-
set hlsearch
set formatoptions+=cro
set wildmode=full

function PrintHighlighter()
  echo "hi<" . synIDattr(synID(line("."),col("."),1),"name") . '> trans<' . synIDattr(synID(line("."),col("."),0),"name") . "> lo<" . synIDattr(synIDtrans(synID(line("."),col("."),1)),"name") . ">"
endfunction

colorscheme solarized
set background=dark
syntax on

set t_Co=16
set notermguicolors

" augroup ft_cpp
"   autocmd!
"   autocmd Syntax cpp syn match cFunctionName "[a-zA-Z_][a-zA-Z_0-9]*("me=e-1 contained
"   autocmd Syntax cpp syn match cFunction "[a-zA-Z_][a-zA-Z_0-9]*(.*{$" contains=cFunctionName,cStorageClass,cType
"   autocmd Syntax cpp hi cFunctionName ctermfg=12
" augroup end

hi StatusLine       ctermfg=8  ctermbg=12
hi StatusLineNC     ctermfg=8  ctermbg=12
hi VertSplit        ctermfg=12 ctermbg=8
hi cppSTLNamespace  ctermfg=12

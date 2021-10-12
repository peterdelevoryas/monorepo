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
set sts=2 ts=2 sw=2 expandtab
"set sts=8 ts=8 sw=8 noexpandtab
set fillchars+=stl:-
set fillchars+=stlnc:-
set hlsearch
set formatoptions+=cro
set wildmode=full
set t_Co=16
set notermguicolors
set noswapfile
set background=dark

" augroup ft_cpp
"   autocmd!
"   autocmd Syntax cpp syn match cFunctionName "[a-zA-Z_][a-zA-Z_0-9]*("me=e-1 contained
"   autocmd Syntax cpp syn match cFunction "[a-zA-Z_][a-zA-Z_0-9]*(.*{$" contains=cFunctionName,cStorageClass,cType
"   autocmd Syntax cpp hi cFunctionName ctermfg=5
" augroup end

augroup ft_c
  autocmd!
  autocmd Syntax c syn keyword Statement let
augroup end

hi Type ctermfg=1
hi StatusLine ctermfg=0 ctermbg=7
hi StatusLineNC ctermfg=0 ctermbg=7
hi VertSplit ctermfg=0 ctermbg=7
hi String ctermfg=6
hi Number ctermfg=4
hi Constant ctermfg=4
hi Statement ctermfg=1
hi Special ctermfg=4
hi Comment ctermfg=8
hi PreProc ctermfg=7
hi Include ctermfg=1
hi cErrInParen ctermfg=7
hi NonText ctermfg=0
hi MatchParen ctermbg=8
hi cppSTLtype ctermfg=7

hi rustFuncName ctermfg=7
hi rustModPathSep ctermfg=7
hi rustModPath ctermfg=7
hi rustFuncCall ctermfg=7
hi rustOperator ctermfg=7
hi rustTrait ctermfg=7
hi rustSigil ctermfg=7
hi rustIdentifier ctermfg=7
hi rustType ctermfg=7
hi rustEnumVariant ctermfg=7

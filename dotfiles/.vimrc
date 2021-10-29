function PrintHighlighter()
  echo "hi<" . synIDattr(synID(line("."),col("."),1),"name") . '> trans<' . synIDattr(synID(line("."),col("."),0),"name") . "> lo<" . synIDattr(synIDtrans(synID(line("."),col("."),1)),"name") . ">"
endfunction

au BufReadPost * if line("'\"") > 1 && line("'\"") <= line("$") | exe "normal! g'\"" | endif

autocmd BufNewFile,BufRead *.pd setfiletype rust

filetype plugin indent on
syntax on
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
set sts=4 ts=4 sw=4 expandtab
set textwidth=80
autocmd FileType cpp setlocal sw=2 ts=2 sts=2 expandtab
autocmd Filetype c setlocal sw=2 ts=2 sts=2 expandtab

augroup ft_c
  autocmd!
  autocmd Syntax c syn keyword Statement let
augroup end

augroup ft_rust
  autocmd!
  autocmd Syntax rust syn keyword Statement function
  "autocmd Syntax rust hi Statement ctermfg=2
  "autocmd Syntax rust hi PreProc ctermfg=12
augroup end

hi StatusLine ctermfg=0 ctermbg=7
hi StatusLineNC ctermfg=0 ctermbg=7
hi VertSplit ctermfg=0 ctermbg=7
hi NonText ctermfg=0
hi MatchParen ctermfg=2 ctermbg=0

hi rustModPath      ctermfg=7
hi rustModPathSep   ctermfg=7
hi rustOperator     ctermfg=7
hi rustFuncName     ctermfg=7
hi rustFuncCall     ctermfg=7
"hi rustFuncName ctermfg=7
"hi rustModPath ctermfg=7
"hi rustFuncCall ctermfg=7
"hi rustTrait ctermfg=7
"hi rustSigil ctermfg=7
"hi rustIdentifier ctermfg=7
"hi rustType ctermfg=7
"hi rustEnumVariant ctermfg=7
"hi rustStorage ctermfg=7
"hi rustLifetime ctermfg=7
"hi rustSelf ctermfg=7
"hi rustEscape ctermfg=13

hi cOctalError ctermfg=0 ctermbg=1

hi markdownH2 ctermfg=7
hi markdownError ctermfg=7

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
"set sts=8 ts=8 sw=8 noexpandtab
set sts=4 ts=4 sw=4 expandtab
"set sts=2 ts=2 sw=2 expandtab
set textwidth=80
colorscheme solarized

augroup ft_c
  autocmd!
  autocmd Syntax c syn keyword Statement let loop
augroup end

augroup ft_cpp
  autocmd!
  autocmd Syntax cpp syn keyword Statement let var loop function fn
augroup end

augroup ft_objcpp
  autocmd!
  autocmd Syntax objcpp syn keyword Statement let var loop
augroup end

augroup ft_rust
  autocmd!
  autocmd Syntax rust syn keyword Statement function
  "autocmd Syntax rust hi Statement ctermfg=2
  "autocmd Syntax rust hi PreProc ctermfg=12
augroup end

"hi Comment      ctermfg=8
"hi StatusLine   ctermfg=0 ctermbg=7
"hi StatusLineNC ctermfg=0 ctermbg=7
"hi VertSplit    ctermfg=0 ctermbg=7
"hi NonText      ctermfg=0
"hi MatchParen   ctermfg=2 ctermbg=0

hi Comment      ctermfg=12
hi StatusLine   ctermfg=8 ctermbg=12
hi StatusLineNC ctermfg=8 ctermbg=12
hi VertSplit    ctermfg=8 ctermbg=12
hi NonText      ctermfg=8
hi MatchParen   ctermfg=2 ctermbg=8

" Minimal highlighting rules.
"hi Type         ctermfg=7
"hi PreProc      ctermfg=7
"hi Constant     ctermfg=7
"hi Statement    ctermfg=7
"hi Special      ctermfg=7
"hi cErrInParen  ctermfg=7
"hi Identifier   ctermfg=7
"hi LineNr       ctermfg=8

" Github Dark highlighting rules.
"hi Include          ctermfg=1
"hi cStorageClass    ctermfg=1
"hi cStructure       ctermfg=1
"hi cType            ctermfg=1
"hi cDefine          ctermfg=1
"hi cStatement       ctermfg=1
"hi cConditional     ctermfg=1
"hi cRepeat          ctermfg=1
"hi cLabel           ctermfg=1
"hi cIncluded        ctermfg=6
"hi cNumber          ctermfg=4
"hi cConstant        ctermfg=4
"hi cString          ctermfg=6
"hi cOperator        ctermfg=7
"hi cTodo            ctermfg=8

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

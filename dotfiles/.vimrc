function PrintHighlighter()
  echo "hi<" . synIDattr(synID(line("."),col("."),1),"name") . '> trans<' . synIDattr(synID(line("."),col("."),0),"name") . "> lo<" . synIDattr(synIDtrans(synID(line("."),col("."),1)),"name") . ">"
endfunction

au BufReadPost * if line("'\"") > 1 && line("'\"") <= line("$") | exe "normal! g'\"" | endif

autocmd BufNewFile,BufRead *.pd setfiletype rust

filetype plugin indent on
syntax on
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
set sts=2 ts=2 sw=2 expandtab
"set sts=4 ts=4 sw=4 expandtab
"set sts=8 ts=8 sw=8 noexpandtab
"autocmd FileType cpp setlocal sw=2 ts=2 sts=2

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

autocmd Syntax cpp syn keyword cppFixedWidthType uint8_t uint16_t uint32_t uint64_t
autocmd Syntax cpp syn keyword cppFixedWidthType int8_t int16_t int32_t int64_t

hi Comment      ctermfg=8
hi StatusLine   ctermfg=0 ctermbg=7
hi StatusLineNC ctermfg=0 ctermbg=7
hi VertSplit    ctermfg=0 ctermbg=7
hi NonText      ctermfg=0
hi MatchParen   ctermfg=2 ctermbg=0
hi Visual       ctermfg=0
hi Todo         ctermfg=8 ctermbg=0

" Minimal highlighting rules.
hi Type         ctermfg=7
hi PreProc      ctermfg=7
hi Constant     ctermfg=7
hi Statement    ctermfg=7
hi Special      ctermfg=7
hi cErrInParen  ctermfg=7
hi Identifier   ctermfg=7
hi LineNr       ctermfg=8

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
"hi cOctalError      ctermfg=0 ctermbg=1
"hi markdownH2       ctermfg=7
"hi markdownError    ctermfg=7
"hi PreProc          ctermfg=1
"hi cppStructure     ctermfg=1
"hi cppOperator      ctermfg=1
"hi cppStorageClass  ctermfg=1
"hi cppNumber        ctermfg=4
"hi cTypedef         ctermfg=1
"hi cppModifier      ctermfg=1
"hi cppType          ctermfg=1
"hi cppStatement     ctermfg=1
"hi cppBoolean       ctermfg=4
"hi cCharacter       ctermfg=6
"hi cFloat           ctermfg=4
"hi cFormat          ctermfg=4
"hi cSpecialCharacter ctermfg=6
"hi cppFixedWidthType ctermfg=4
"hi cppConstant      ctermfg=4
"hi cUserLabel       ctermfg=1

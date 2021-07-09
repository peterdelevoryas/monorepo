

set smartcase
set ignorecase
set smartindent
set cinoptions=(0

set sts=2 ts=8 sw=2 expandtab

au BufRead,BufNewFile *.h set filetype=c
"au BufRead,BufNewFile *.pd set filetype=c

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

set background=dark
syntax on

set t_Co=16
set notermguicolors

hi VertSplit ctermfg=0 ctermbg=7
hi StatusLine ctermfg=0 ctermbg=7
hi StatusLineNC ctermfg=0 ctermbg=7
hi Search ctermfg=0
hi Visual ctermfg=0 cterm=none
hi NonText ctermfg=0
hi Type ctermfg=7
hi Constant ctermfg=7
hi Statement ctermfg=7
hi Comment ctermfg=8

"hi Type ctermfg=7
"hi Constant ctermfg=6
"hi Include ctermfg=1
"hi Comment ctermfg=8
"hi Statement ctermfg=1
"hi PreProc ctermfg=7

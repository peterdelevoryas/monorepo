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
set fillchars+=stl:-
set fillchars+=stlnc:-
set hlsearch
set formatoptions+=cro
set wildmode=full
set t_Co=16
set notermguicolors
set noswapfile
set background=dark

" Settings for Solarized
" colorscheme solarized
" hi StatusLine ctermfg=8 ctermbg=12
" hi StatusLineNC ctermfg=8 ctermbg=12
" hi VertSplit ctermfg=12 ctermbg=8

" Settings for Github Dark
augroup ft_rust
  autocmd!
  autocmd Syntax rust syn keyword Statement ret
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
hi rustStorage ctermfg=7
hi rustLifetime ctermfg=7
hi rustSelf ctermfg=7

hi cOctalError ctermfg=0 ctermbg=1

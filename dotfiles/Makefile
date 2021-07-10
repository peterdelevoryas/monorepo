all:

.PHONY: sync install

install:
	cp .tmux.conf $(HOME)/.tmux.conf
	cp .vimrc $(HOME)/.vimrc
	cp .alacritty.yml $(HOME)/.alacritty.yml
	mkdir -p $(HOME)/.vim/colors
	cp solarized.vim $(HOME)/.vim/colors/

sync:
	cp $(HOME)/.vimrc .vimrc
	cp $(HOME)/.alacritty.yml .alacritty.yml
	cp $(HOME)/.tmux.conf .tmux.conf

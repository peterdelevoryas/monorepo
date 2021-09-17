all:

.PHONY: install

install:
	ln -sF $(realpath .tmux.conf) $(HOME)/.tmux.conf
	ln -sF $(realpath .vimrc) $(HOME)/.vimrc
	ln -sF $(realpath .alacritty.yml) $(HOME)/.alacritty.yml
	mkdir -p $(HOME)/.vim/colors
	cp solarized.vim $(HOME)/.vim/colors

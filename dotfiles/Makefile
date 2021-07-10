all: .vimrc .alacritty.yml

.PHONY: sync install

install: .vimrc .alacritty.yml
	cp $(HOME)/.vimrc /tmp/.vimrc.prev
	cp $(HOME)/.alacritty.yml /tmp/.alacritty.yml
	cp .vimrc $(HOME)/.vimrc
	cp .alacritty.yml $(HOME)/.alacritty.yml

sync:
	cp $(HOME)/.vimrc .vimrc
	cp $(HOME)/.alacritty.yml .alacritty.yml

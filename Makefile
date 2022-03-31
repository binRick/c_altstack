BUILD_DIR = .build
MAKE_LOG = /tmp/make.log
MAKE_LOG = /tmp/ninja.log
PASSH_BUILD = passh -L $(MAKE_LOG)
PASSH_NINJA = passh -L $(NINJA_LOG)
DEBUG = passh rg error: $(NINJA_LOG) --no-line-number --no-filename -A 2 | command bat --style=plain --color=always --theme=1337 --paging=always --italic-text=always

CLIB_MODULES = binRick/c_list binRick/c_base64 binRick/c_aes binRick/c_optparse

default: all

.PHONY: all

all: build test

.PHONY: .FORCE
.FORCE:

clib:
	@clib install
	@clib install --save -f $(CLIB_MODULES)

setup: clib

deps:
	@test -d deps || clib install

build: deps
	@test -d $(BUILD_DIR) && { meson $(BUILD_DIR) --reconfigure; } || { meson $(BUILD_DIR); }

test:
	@meson test -C $(BUILD_DIR) --verbose

rm_bd:
	@test -d $(BUILD_DIR) && rm -rf $(BUILD_DIR)

rm_deps:
	@test -d deps && rm -rf deps

clean: rm_deps rm_bd setup

debug: build
	@$(PASSH_NINJA) ninja -C $(BUILD_DIR)

install:
	@echo Install OK

tidy:
	@./tidy.sh

pull: git-pull clib build test

git-pull:
	@git pull

commit: tidy
	@git commit -am 'automated git commit'
	@passh git status

push: tidy commit
	@git push

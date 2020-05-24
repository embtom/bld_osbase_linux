#
# This file is part of the EMBTOM project
# Copyright (c) 2018-2020 Thomas Willetal 
# (https://github.com/embtom)
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

build_debug_dir := build/Debug
build_release_dir := build/Release

build_debug_make := $(build_debug_dir)/Makefile
build_release_make := $(build_release_dir)/Makefile

releasePacket = $(wildcard $(build_release_dir)/*.deb)
debugPacket = $(wildcard $(build_debug_dir)/*.deb)

#############################################################
# Main build targets
############################################################# 
all: release_build debug_build
clean: debug_clean release_clean
distclean: release_distclean debug_distclean
install: install_release

#############################################################
# Sub build targets (Release)
#############################################################
release_build: $(build_release_make) 
	make -C $(dir $<) -j16 all
	make -C $(dir $<) package

release_clean: 
	make -C$(dir $(build_release_make)) clean
	
release_distclean:
	rm $(build_release_dir) -R 

$(build_release_make):
	mkdir -p $(@D)
	cd $(@D) && cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DLIB_LIST_LOCK=lock_cas ../..

#############################################################
# Sub build targets (Debug)
#############################################################
debug_build: $(build_debug_make) 
	make -C $(dir $<) -j16 all
	make -C $(dir $<) package

debug_clean: 
	make -C$(dir $(build_debug_make)) clean

debug_distclean:
	rm $(build_debug_dir) -R
	
$(build_debug_make):
	mkdir -p $(@D)
	cd $(@D) && cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DLIB_LIST_LOCK=lock_cas ../..

#############################################################
# Packet install targets 
#############################################################
install_release:
	dpkg -r osBase-release > /dev/null
	dpkg -r osBase-debug > /dev/null
	dpkg -i $(releasePacket)

install_debug:
	dpkg -r osBase-release > /dev/null
	dpkg -r osBase-debug > /dev/null
	dpkg -i $(debugPacket)
		

# Dependencies
# brew install lld
# brew install llvm
#

#test.wasm: test.wat
#	wat2wasm test.wat

dist/src.wat: dist/src.wasm
	wasm2wat dist/src.wasm -o dist/src.wat

dist/src.wasm: src.c
	clang --target=wasm32 \
	--no-standard-libraries \
	-Wl,--no-entry \
	-Wl,--allow-undefined \
	-Wl,--export=c_func \
	-o dist/src.wasm \
	src.c



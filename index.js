import fs from "fs";

const wasmFile = fs.readFileSync("./dist/src.wasm");

WebAssembly.instantiate(wasmFile).then((result) => {
  console.log(result.instance.exports.c_func);
});

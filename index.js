import fs from "fs";
let memory = undefined;

const wasmFile = fs.readFileSync("./dist/src.wasm");

const logger = (val) => {
  const length = new Uint8Array(memory.buffer, val, 1)[0];
  console.log(length);
  const buffer = new Uint8Array(memory.buffer, val + 1, length);
  console.log(new TextDecoder("utf8").decode(buffer));
};

const importObj = {
  env: { logger },
};
WebAssembly.instantiate(wasmFile, importObj).then((result) => {
  memory = result.instance.exports.memory;
  result.instance.exports.c_func();
});

let memory = undefined;
let g_keyPressed = 0;
let g_next_frame_cb = undefined;
let g_dt = 0;

const wasmFile = fetch("./src.wasm");

const logCStr = (str_p) => {
  const buffer = new Uint8Array(memory.buffer, str_p);
  let string = "string: '";
  for (let i = 0; buffer[i] != 0; i++) {
    string += String.fromCharCode(buffer[i]);
  }
  string += "'";
  console.log(string);
};

const logVector2D = (v_p) => {
  const buffer = new Float32Array(memory.buffer, v_p, 2);
  console.log(`vector2d: (${buffer[0]}, ${buffer[1]})`);
};

const logValue = (v) => {
  console.log(`value: ${v}`);
};

const importObj = {
  env: { logVector2D, logCStr, logValue },
};

let prevTimeStamp = 0;
function nextFrame(t_ms) {
  if (prevTimeStamp == 0) {
    prevTimeStamp = t_ms;
  }
  g_dt = (t_ms - prevTimeStamp) / 1000;
  prevTimeStamp = t_ms;
  g_next_frame_cb();
  console.log(g_dt);
  requestAnimationFrame(nextFrame);
}

window.onload = () => {
  const canvas = document.getElementById("canvas");
  const body = document.getElementById("body");
  canvas.style.position = "absolute";
  canvas.style.width = "800px";
  canvas.style.height = "600px";
  canvas.style.top = "calc(50% - 300px)";
  canvas.style.left = "calc(50% - 400px)";
  canvas.style.backgroundColor = "blue";
  WebAssembly.instantiateStreaming(wasmFile, importObj).then((result) => {
    memory = result.instance.exports.memory;
    body.onkeydown = (ev) => {
      ev.preventDefault();
      result.instance.exports.engine_key_down(ev.keyCode);
    };
    body.onkeyup = (ev) => {
      ev.preventDefault();
      result.instance.exports.engine_key_up(ev.keyCode);
    };
    result.instance.exports.engine_init();
    g_next_frame_cb = result.instance.exports.engine_update;
    requestAnimationFrame(nextFrame);
  });
};

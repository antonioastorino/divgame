let memory = undefined;
let g_keyPressed = 0;
let g_next_frame_cb = undefined;
let g_get_wall_rect = undefined;
let g_dt = 0;
let g_wall_pairs = [];
let g_window_height = 0;
let g_window_width = 0;
let g_wall_max_z = 0;

const WALL_SEPARATION = 300;

class WallPair {
  leftWall;
  rightWall;
  constructor(canvas) {
    this.leftWall = document.createElement("div");
    this.leftWall.style.position = "absolute";
    this.leftWall.style.backgroundColor = "white";
    this.rightWall = document.createElement("div");
    this.rightWall.style.position = "absolute";
    this.rightWall.style.backgroundColor = "white";

    canvas.appendChild(this.leftWall);
    canvas.appendChild(this.rightWall);
  }

  updatePosition(x, y, z, w, h) {
    this.leftWall.style.left = `${x - WALL_SEPARATION / z}px`; // position.x
    this.leftWall.style.bottom = `${y}px`; // position.y
    this.leftWall.style.width = `${w}px`; // size.w
    this.leftWall.style.height = `${h}px`; // size.h
    this.rightWall.style.left = `${x + WALL_SEPARATION / z}px`; // position.x
    this.rightWall.style.bottom = `${y}px`; // position.y
    this.rightWall.style.width = `${w}px`; // size.w
    this.rightWall.style.height = `${h}px`; // size.h
  }
}

const wasmFile = fetch("./src.wasm");

const jsLogCStr = (str_p) => {
  const buffer = new Uint8Array(memory.buffer, str_p);
  let string = "string: '";
  for (let i = 0; buffer[i] != 0; i++) {
    string += String.fromCharCode(buffer[i]);
  }
  string += "'";
  console.log(string);
};

const jsLogVector3D = (v_p) => {
  const buffer = new Float32Array(memory.buffer, v_p, 3);
  console.log(`vector3d: (${buffer[0]}, ${buffer[1]}, ${buffer[2]})`);
};

const jsLogInt = (v) => {
  console.log(`int: ${v}`);
};

const jsLogFloat = (v) => {
  console.log(`float: ${v}`);
};

function jsUpdateWallRect(struct_p) {
  const [x, y, z, w, h] = new Float32Array(memory.buffer, struct_p, 5);
  g_wall_pairs[0].updatePosition(x, y, z, w, h);
}

function jsSetEngineParams(params_p) {
   [g_window_height, g_window_width, g_wall_max_z] = new Int32Array(memory.buffer, params_p, 3);
   console.log(g_window_width)
}

let prevTimeStamp = 0;
function nextFrame(t_ms) {
  if (prevTimeStamp == 0) {
    prevTimeStamp = t_ms;
  }
  g_dt = (t_ms - prevTimeStamp) / 1000;
  if (g_dt > 0.05) {
    g_dt = 0.05;
  }
  prevTimeStamp = t_ms;
  g_next_frame_cb();
  requestAnimationFrame(nextFrame);
}

function applyWallStyle(wall) {
  wall.style.position = "absolute";
  wall.style.backgroundColor = "white";
}

function jsGetDt() {
  return g_dt;
}

const importObj = {
  env: { jsLogVector3D, jsLogCStr, jsLogInt, jsLogFloat, jsGetDt, jsSetEngineParams, jsUpdateWallRect },
};

window.onload = () => {
  const canvas = document.getElementById("canvas");
  const body = document.getElementById("body");
  canvas.style.position = "absolute";
  canvas.style.backgroundColor = "blue";

  g_wall_pairs.push(new WallPair(canvas));

  WebAssembly.instantiateStreaming(wasmFile, importObj).then((result) => {
    memory = result.instance.exports.memory;
    result.instance.exports.engine_init();
    canvas.style.width = `${g_window_width}px`;
    canvas.style.height = `${g_window_height}px`;
    canvas.style.top = `calc(50% - ${g_window_height / 2}px)`;
    canvas.style.left = `calc(50% - ${g_window_width / 2}px)`;
    body.onkeydown = (ev) => {
      ev.preventDefault();
      result.instance.exports.engine_key_down(ev.keyCode);
    };
    body.onkeyup = (ev) => {
      ev.preventDefault();
      result.instance.exports.engine_key_up(ev.keyCode);
    };
    g_next_frame_cb = result.instance.exports.engine_update;
    requestAnimationFrame(nextFrame);
  });
};


import { wasm_import_base } from "./bind/wasm_base.js";
import { wasm_import_gl } from "./bind/wasm_gl.js";
import { wasm_import_stdio } from "./bind/wasm_stdio.js";

class Game {

  constructor() {
    this.wasm = null; // wasm instance
    this.gl = null;
    this.data_id = 0;
    this.data = {};
    this.initialized = false;
    this.shader_vertex = null;
    this.shader_fragment = null;
    this.await_count = 0;
    this.ready = false;
    this.frame_time = 0;

    this.utf8_decoder = new TextDecoder("utf-8");
  }

  memory(index, size) {
    if (this.initialized) {
      let array = new Uint8Array(this.wasm.exports.memory.buffer);
      return array.subarray(index, index + size);
    }
    return null;
  }

  memory_f(index, size_f) {
    if (this.initialized) {
      let array = new Float32Array(this.wasm.exports.memory.buffer);
      return array.subarray(index/4, index/4 + size_f);
    }
    return null;
  }

  clone_memory(index, size) {
    if (this.initialized)
      return this.wasm.exports.memory.buffer.slice(index, size);
    return null;
  }

  str(index, size) {
    let arr = this.memory(index, size);
    return this.utf8_decoder.decode(arr);
  }

  store(data) {
    this.data[++this.data_id] = data;
    return this.data_id;
  }

  free(data_id) {
    delete this.data[data_id];
  }

  async initialize(wasm_filename) {
    await this.initialize_wasm(wasm_filename);
    this.initialize_webgl();
    this.initialize_window_events();
  }

  async initialize_wasm(wasm_filename) {
    const response = await fetch(wasm_filename);
    let imports = {};

    wasm_import_base(imports, this);
    wasm_import_stdio(imports, this);
    wasm_import_gl(imports, this);

    await WebAssembly.instantiateStreaming(response, { env: imports })
    .then((obj) => {
      this.wasm = obj.instance;
      if (!this.wasm) return;
      this.initialized = true;
      this.wasm.exports.canary(2);
    });
  }

  initialize_webgl() {
    const canvas = document.getElementById("game");
    canvas.width = canvas.parentElement.offsetWidth;
    canvas.height = canvas.parentElement.offsetHeight;
    let gl = canvas.getContext("webgl2");

    if (!gl) {
      alert("Unable to initialize WebGL :(");
    }

    gl.clearColor(0.05, 0.15, 0.25, 1);
    gl.clearDepth(1);
    gl.enable(gl.DEPTH_TEST);
    gl.enable(gl.CULL_FACE);
    gl.depthFunc(gl.LEQUAL);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    this.gl = gl;
  }

  initialize_window_events() {
    let game = this;

    let canvas_resize = () => {
      let pr = window.devicePixelRatio;
      let width = (window.innerWidth * pr) | 0;
      let height = (window.innerHeight * pr) | 0;
      // gl.canvas.style.width = w + "px";
      // gl.canvas.style.height = h + "px";
      game.gl.canvas.width = width;
      game.gl.canvas.height = height;
      game.wasm.exports.wasm_push_window_event(0x206, width, height);
    }

    canvas_resize();
    window.addEventListener("resize", canvas_resize);
  }
}

export { Game };


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
    await this.initialize_webgl();
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
      if (!this.wasm) return null;
      this.initialized = true;
      this.wasm.exports.canary(2);
    });
  }

  async initialize_webgl() {
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
    gl.depthFunc(gl.LEQUAL);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    this.gl = gl;
  }

  //async initialize_game() {
    ///////////////////////////////
    /*
    await fetch("./res/shaders/basic.vert")
      .then((res) => res.text())
      .then((text) => {
        game.shader_vertex = text;
      }).catch((e) => console.log(e));

    await fetch("./res/shaders/basic.frag")
      .then((res) => res.text())
      .then((text) => {
        game.shader_fragment = text;
      }).catch((e) => console.log(e));

    function loadShader(type, src) {
      const shader = gl.createShader(type);

      gl.shaderSource(shader, src);
      gl.compileShader(shader);

      if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        alert('Error compiling shaders: ' + gl.getShaderInfoLog(shader));
        gl.deleteShader(shader);
        return null;
      }

      return shader;
    }

    function initShaderProgram() {
      const vert = loadShader(gl.VERTEX_SHADER, game.shader_vertex);
      const frag = loadShader(gl.FRAGMENT_SHADER, game.shader_fragment);

      const program = gl.createProgram();
      gl.attachShader(program, vert);
      gl.attachShader(program, frag);
      gl.linkProgram(program);

      if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
        alert('Unable to initialize shader: ' + gl.getProgramInfoLog(program));
        return null;
      }

      return program;
    }

    this.shader = initShaderProgram();
    //*/
    ///////////////////////////////
  //}
}

export { Game };

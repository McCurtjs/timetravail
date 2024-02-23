import { types } from "./wasm_const.js";

function wasm_import_gl(imports, game) {

  imports["glGetError"] = () => {
    return game.gl.getError();
  }

  // Shaders

  imports["glCreateShader"] = (shader_type) => {
    return game.store({
      type: types.shader,
      ready: true,
      shader: game.gl.createShader(shader_type),
    });
  }

  imports["js_glShaderSource"] = (data_id, src, len) => {
    let data = game.data[data_id];
    if (!data || data.type != types.shader) return;
    game.gl.shaderSource(data.shader, game.str(src, len));
  }

  imports["glCompileShader"] = (data_id) => {
    let data = game.data[data_id];
    if (!data || data.type != types.shader) return;
    game.gl.compileShader(data.shader);
  }

  imports["js_glGetShaderParameter"] = (data_id, pname) => {
    let data = game.data[data_id];
    if (!data || data.type != types.shader) return;
    return game.gl.getShaderParameter(data.shader, pname) ? 1 : 0;
  };

  imports["glDeleteShader"] = (data_id) => {
    let data = game.data[data_id];
    if (!data || data.type != types.shader) return;
    game.gl.deleteShader(data.shader);
    game.free(data_id);
  }

  imports["glCreateProgram"] = () => {
    return game.store({
      type: types.sprog,
      ready: true,
      program: game.gl.createProgram(),
    });
  }

  imports["glAttachShader"] = (program_id, shader_id) => {
    let p_data = game.data[program_id];
    let s_data = game.data[shader_id];
    if (!p_data || p_data.type != types.sprog) return;
    if (!s_data || s_data.type != types.shader) return;
    game.gl.attachShader(p_data.program, s_data.shader);
  }

  imports["glLinkProgram"] = (data_id) => {
    let data = game.data[data_id];
    if (!data || data.type != types.sprog) return;
    game.gl.linkProgram(data.program);
  }

  imports["js_glGetProgramParameter"] = (data_id, pname) => {
    let data = game.data[data_id];
    if (!data || data.type != types.sprog) return;
    return game.gl.getProgramParameter(data.program, pname) ? 1 : 0;
  };

  imports["glUseProgram"] = (data_id) => {
    let data = game.data[data_id];
    if (!data || data.type != types.sprog) return;
    game.gl.useProgram(data.program);
  }

  // Buffers

  imports["js_glCreateBuffer"] = () => {
    return game.store({
      type: types.buffer,
      ready: true,
      buffer: game.gl.createBuffer(),
    });
  }

  imports["glBindBuffer"] = (target, data_id) => {
    let data = game.data[data_id];
    if (!data || data.type != types.buffer) return;
    game.gl.bindBuffer(target, data.buffer);
  }

  imports["glBufferData"] = (target, size, src, usage) => {
    game.gl.bufferData(target, game.memory(src, size), usage);
  }

  imports["glVertexAttribPointer"] = (index, size, type, norm, stride, ptr) => {
    game.gl.vertexAttribPointer(index, size, type, norm, stride, ptr);
  }

  imports["glEnableVertexAttribArray"] = (index) => {
    game.gl.enableVertexAttribArray(index);
  }

  imports["glDisableVertexAttribArray"] = (index) => {
    game.gl.disableVertexAttribArray(index);
  }

  imports["glDrawArrays"] = (mode, first, count) => {
    game.gl.drawArrays(mode, first, count);
  }
}

export { wasm_import_gl };

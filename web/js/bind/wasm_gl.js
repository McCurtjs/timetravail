import { types } from "./wasm_const.js";

function wasm_import_gl(imports, game) {

  imports["glGetError"] = () => {
    return game.gl.getError();
  }

  imports["glViewport"] = (x, y, width, height) => {
    game.gl.viewport(x, y, width, height);
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

  // Buffers and VAO

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

  imports["glDeleteBuffer"] = (data_id) => {
    let data = game.data[data_id];
    if (!data || data.type != types.buffer) return;
    game.gl.deleteBuffer(data.buffer);
    game.free(data_id);
  }

  imports["js_glCreateVertexArray"] = () => {
    return game.store({
      type: types.vao,
      ready: true,
      vao: game.gl.createVertexArray(),
    })
  }

  imports["glBindVertexArray"] = (data_id) => {
    let data = game.data[data_id];
    if (!data || data.type != types.vao) return;
    game.gl.bindVertexArray(data.vao);
  }

  imports["glDeleteVertexArray"] = (data_id) => {
    let data = game.data[data_id];
    if (!data || data.type != types.vao) return;
    game.gl.deleteVertexArray(data.vao);
    game.free(data_id);
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

  imports["js_glGetUniformLocation"] = (program_id, name, len) => {
    let data = game.data[program_id];
    if (!data || data.type != types.sprog) return 0;
    // TODO: also store ref in program data so we can delete them later
    return game.store({
      type: types.uniform,
      ready: true,
      location: game.gl.getUniformLocation(data.program, game.str(name, len))
    });
  }

  imports["glUniform4fv"] = (loc_id, count, ptr) => {
    let data = game.data[loc_id];
    if (!data || data.type != types.uniform) return 0;
    let bytes = game.memory_f(ptr, count * 4);
    game.gl.uniform4fv(data.location, bytes, 0);
  }

  imports["glUniformMatrix4fv"] = (loc_id, count, transpose, ptr) => {
    let data = game.data[loc_id];
    if (!data || data.type != types.uniform) return 0;
    let bytes = game.memory_f(ptr, count * 16);
    game.gl.uniformMatrix4fv(data.location, transpose != 0, bytes, 0);
  }
}

export { wasm_import_gl };

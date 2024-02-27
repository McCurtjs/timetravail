import { Game } from "./game.js";

var game = null;


function renderTimer(now) {
  const { await_count, gl } = game;
  const { wasm_load, wasm_update, wasm_render } = game.wasm.exports;
  const dt = now - game.frame_time;
  game.frame_time = now;

  document.getElementById('console').textContent = `
    WASM GL Test - FPS: ${Math.floor(1000 / dt)}
  `;

  if (!game.ready) {
    if (wasm_load(await_count)) {
      game.ready = true;
    }
  } else {
    wasm_update(dt / 1000);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    wasm_render();
  }
  requestAnimationFrame(renderTimer);
}

window.onload = async() => {
  game = new Game();

  await game.initialize('test.wasm');

  game.wasm.exports.wasm_preload();

  requestAnimationFrame(renderTimer);

  console.log('done');
}

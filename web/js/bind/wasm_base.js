
function wasm_import_base(imports, game) {
  const utf8_decoder = new TextDecoder("utf-8");

  imports['js_alert'] = (str, len) => {
    window.alert(game.str(str, len));
  }

  imports['js_log'] = (str, len) => {
    console.log(game.str(str, len));
  }

  imports['js_log_int'] = (i) => {
    console.log(i);
  }

  imports['js_log_num'] = (i) => {
    console.log(i);
  }
}

export { wasm_import_base };

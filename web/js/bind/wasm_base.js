
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

  imports['js_log_num_array'] = (ptr, count) => {
    let arr = [...game.memory_f(ptr, count)];
    arr = arr.map((f) => {
      return parseFloat(f.toFixed(4));
    });
    console.log(arr);
    //console.log([...game.memory_f(ptr, count)]);
  }

  //imports['js_log_lines'] = (ptr, count) => {
  //  let arr = [];
  //}

//  imports['js_log_bytes'] = (ptr, count) => {
//    console.log([...game.memory(ptr, count)]);
//  }
}

export { wasm_import_base };

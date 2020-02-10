let read_json = path =>
  try(path |> Fpath.to_string |> Yojson.Safe.from_file) {
  | Yojson.Json_error(msg) => raise(Errors.Json_parse_error(path, msg))
  };

let normalize_dir_path = cwd => 
  (cwd |> Fpath.to_string |> Sys.is_directory ? cwd : cwd |> Fpath.parent)
  |> Fpath.normalize
  |> Fpath.to_dir_path;


let rec ls_dir = (path: Fpath.t) => {
  let s_path = path |> Fpath.to_string;
  Sys.is_directory(s_path)
    ? Sys.readdir(s_path)
      |> Array.to_list
      |> List.map(seg => Fpath.add_seg(path, seg) |> ls_dir)
      |> List.flatten
    : [path];
};
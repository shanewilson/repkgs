type path_type =
  | Dir(Fpath.t)
  | File(Fpath.t)
  | Dne;

let exists = path => {
  let exists = path |> Bos.OS.Path.exists;
  let is_dir = path |> Bos.OS.Dir.exists;

  switch (exists, is_dir) {
  | (Ok(true), Ok(true)) => Ok(Dir(path))
  | (Ok(true), Ok(false)) => Ok(File(path))
  | (Ok(false), _) => Ok(Dne)
  | (Error(`Msg(msg)), _)
  | (_, Error(`Msg(msg))) => raise(Errors.Fs_error(path, msg))
  };
};

let read = path =>
  path
  |> Bos.OS.File.read
  |> (
    fun
    | Ok(data) => data
    | Error(`Msg(msg)) => raise(Errors.Fs_error(path, msg))
  );
  
let read_json = path =>
  path
  |> read
  |> (
    data =>
      try(data |> Ezjsonm.from_string) {
      | Ezjsonm.Parse_error(_, msg) =>
        raise(Errors.Json_parse_error(path, msg))
      }
  );
let read_yaml = path =>
  path
  |> read
  |> (
    data =>
      data |> Yaml.of_string |> fun
      | Ok(wj) => wj
      | Error(err) => {
          raise(
            Errors.Json_parse_error(path, "beep"),
          );
        }
  );

let get_cwd = () =>
  switch (Bos.OS.Dir.current()) {
  | Ok(cwd) => cwd
  | Error(`Msg(msg)) => raise(Errors.Fs_cwd(msg))
  };

let normalize_dir_path = cwd => {
  (
    switch (cwd |> exists) {
    | Ok(Dir(path)) => path
    | Ok(File(path)) => path |> Fpath.parent
    | Ok(Dne) => raise(Errors.Fs_dne(cwd))
    | Error(`Msg(msg)) => raise(Errors.Fs_error(cwd, msg))
    }
  )
  |> Fpath.normalize
  |> Fpath.to_dir_path;
};

let normalize_cwd = cwd =>
  Fpath.v(cwd) |> Fpath.append(get_cwd()) |> normalize_dir_path;

let rec ls_dir = (path: Fpath.t) =>
  switch (path |> exists) {
  | Ok(File(p)) => [p]
  | Ok(Dir(p)) =>
    p
    |> Bos.OS.Dir.contents
    |> (
      fun
      | Error(`Msg(msg)) => raise(Errors.Fs_error(p, msg))
      | Ok(ls) =>
        ls |> List.map(seg => Fpath.append(p, seg) |> ls_dir) |> List.flatten
    )
  | Ok(Dne) => raise(Errors.Fs_dne(path))
  | Error(`Msg(msg)) => raise(Errors.Fs_error(path, msg))
  };
type path_type =
  | Dir(Fpath.t)
  | File(Fpath.t)
  | Dne;

let exists = path => {
  let exists = path |> OS.Path.exists;
  let is_dir = path |> OS.Dir.exists;

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
  |> OS.File.read
  |> (
    fun
    | Ok(data) => data
    | Error(`Msg(msg)) => raise(Errors.Fs_error(path, msg))
  );

let yaml = path =>
  path
  |> read
  |> Yaml.of_string
  |> (
    fun
    | Ok(data) => data
    | Error(`Msg(msg)) => raise(Errors.Fs_error(path, msg))
  );

let read_and_parse = (path, ~parser) => {
  path
  |> yaml
  |> parser
  |> (
    fun
    | Ok(wj) => wj
    | Error(err) => {
        raise(Errors.Fs_parse_error(path, err));
      }
  );
};

let get_cwd = () =>
  switch (OS.Dir.current()) {
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

let ls_dir = (cwd, ~fn) =>
  Bos.OS.Path.fold(
    ~elements=`Files,
    ~traverse=
      `Sat(
        path => {
          Ok(
            !(
              ["node_modules"]
              |> List.exists(x => compare(x, path |> Fpath.filename) == 0)
            ),
          )
        },
      ),
    fn,
    [],
    [cwd],
  )
  |> (
    fun
    | Ok(r) => r
    | Error(`Msg(msg)) => raise(Errors.Fs_error(cwd, msg))
  );
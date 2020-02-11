module Workspace = {
  type t =
    | Package(Fpath.t)
    | WorkTree(Fpath.t, list(Fpath.t))
    | Root(Fpath.t, list(Fpath.t));

  let to_path =
    fun
    | Root(x, _) => x
    | Package(x) => x
    | WorkTree(x, _) => x;

  let path_to_manifest = (path, manifest_file) => manifest_file |> Fpath.append(path |> Fs.normalize_dir_path);

  let normalize_patterns = (cwd, patterns) =>
    patterns
    |> List.map(glob =>
         Fpath.add_seg(Fpath.append(cwd, Fpath.v(glob)), "package.json")
       );

  let check_workspace_type =
      (path, ~path_to_manifest, ~read_parse_manifest, ~get_workspace_patterns) =>
    switch (path |> path_to_manifest |> Fs.exists) {
    | Ok(File(p)) =>
      p
      |> read_parse_manifest
      |> get_workspace_patterns
      |> (
        fun
        | Some(patterns) =>
          Ok(WorkTree(path, patterns |> normalize_patterns(path)))
        | None => Ok(Package(path))
      )
    | _ => Ok(Package(path))
    };

  let filter_ls_dir = (patterns, dirs) =>
    dirs
    |> List.filter(pth =>
         patterns
         |> List.exists(pattern =>
              Glob.test(
                Glob.of_string(pattern |> Fpath.to_string),
                pth |> Fpath.to_string,
              )
            )
       );

  let rec get_dirs = (cwd, patterns, ~nested=false, ~check_workspace_type, ()) => {
    let dirs = cwd |> Fs.ls_dir |> filter_ls_dir(patterns);

    dirs
    |> List.map(
         find_matching_dirs(_, ~nested=true, ~check_workspace_type, ()),
       )
    |> List.flatten
    |> List.cons(nested ? WorkTree(cwd, patterns) : Root(cwd, patterns));
  }
  and find_matching_dirs = (cwd, ~nested=false, ~check_workspace_type, ()) => {
    let cwd = cwd |> Fs.normalize_dir_path;

    cwd
    |> check_workspace_type
    |> (
      fun
      | Ok(WorkTree(path, patterns)) =>
        get_dirs(path, patterns, ~nested, ~check_workspace_type, ())
      | _ => [Package(cwd)]
    );
  };

  let rec find_root_dir =
          (path, ~check_workspace_type, ~closest=None, ~depth=3, ()) => {
    let stop =
      fun
      | Some(dir) => {
          Logs.debug(m =>
            m("Found root directory of workspace: %s", dir |> Fpath.to_string)
          );
          dir;
        }
      | None => raise(Errors.No_workspace_found);

    let hit_max_depth = depth <= 0;

    let hit_fs_root = path |> Fpath.normalize |> Fpath.is_root;

    Logs.debug(m =>
      m("Looking for manifest in directory: %s", path |> Fpath.to_string)
    );

    switch (hit_max_depth, hit_fs_root) {
    | (true, _) =>
      Logs.debug(m =>
        m("Stopping because we hit max depth without finding a worktree.")
      );
      stop(closest);
    | (_, true) =>
      Logs.debug(m => m("Stopping because we hit the FS root."));
      stop(closest);
    | _ =>
      let frd = find_root_dir(path |> Fpath.parent, ~check_workspace_type);

      path
      |> check_workspace_type
      |> (
        fun
        | Ok(WorkTree(p, _)) => {
            Logs.debug(m =>
              m("Detected worktree at: %s", p |> Fpath.to_string)
            );
            frd(~closest=Some(p), ~depth=3, ());
          }
        | _ => frd(~closest, ~depth=depth - 1, ())
      );
    };
  };
};

module Yarn = {
  [@deriving of_yojson({strict: false})]
  type t = {
    [@default None]
    workspaces: option(list(string)),
  };

  let manifest_file = Fpath.v("package.json");

  let path_to_manifest = path => Workspace.path_to_manifest(path, manifest_file)

  let read_manifest = path => path |> Fs.read_json;

  let parse_manifest = manifest => manifest |> of_yojson;

  let read_parse_manifest = path =>
    path
    |> read_manifest
    |> parse_manifest
    |> (
      fun
      | Ok(wj) => wj
      | Error(err) => {
          raise(Errors.Json_parse_error(path, err));
        }
    );

  let get_workspace_patterns = manifest => manifest.workspaces;

  let check_workspace_type = path => 
    path
    |> Workspace.check_workspace_type(
         ~path_to_manifest,
         ~read_parse_manifest,
         ~get_workspace_patterns,
       );
  

  let find_workspace_dirs = cwd =>
    Workspace.find_matching_dirs(cwd, ~check_workspace_type, ());

  let rec find_root_dir = Workspace.find_root_dir(~check_workspace_type);

  let detect = cwd => {
    Logs.debug(m => m("Checking if this is a Yarn managed workspace..."));

    try(find_root_dir(cwd, ()) |> (x => Some(x))) {
    | _ => None
    };
  };
};

type t =
  | Yarn;

let to_string =
  fun
  | Yarn => "Yarn";

let managers = [(Yarn.detect, Yarn)];

let detect_workspace_manager = cwd =>
  managers
  |> List.fold_left(
       (acc, (detect, kind)) => {
         cwd
         |> detect
         |> (
           fun
           | Some(root) => {
               Logs.info(m =>
                 m("Detected workspace manager: %s", kind |> to_string)
               );

               Some((kind, root));
             }
           | None => acc
         )
       },
       None,
     );

let find_root_dir = (wsmgr, cwd) =>
  switch (wsmgr) {
  | Yarn => cwd |> Yarn.find_root_dir
  };

let find_workspace_dirs = (wsmgr, cwd) =>
  switch (wsmgr) {
  | Yarn => cwd |> Yarn.find_workspace_dirs
  };
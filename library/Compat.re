module Workspace = {
  type t =
    | Root(Fpath.t)
    | Package(Fpath.t)
    | WorkTree(Fpath.t);

  let to_path =
    fun
    | Root(x) => x
    | Package(x) => x
    | WorkTree(x) => x;

  let normalize_patterns = (cwd, patterns) =>
    patterns
    |> List.map(glob =>
         Fpath.add_seg(Fpath.append(cwd, Fpath.v(glob)), "package.json")
       );

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

  let rec get_dirs =
          (
            cwd,
            manifest,
            ~nested=false,
            ~get_workspace_patterns,
            ~read_parse_manifest,
            ~is_worktree,
            (),
          ) => {
    let patterns =
      manifest |> get_workspace_patterns |> normalize_patterns(cwd);

    let dirs = cwd |> Fs.ls_dir |> filter_ls_dir(patterns);

    dirs
    |> List.map(
         find_matching_dirs(
           _,
           ~nested=true,
           ~get_workspace_patterns,
           ~read_parse_manifest,
           ~is_worktree,
           (),
         ),
       )
    |> List.flatten
    |> List.cons(nested ? WorkTree(cwd) : Root(cwd));
  }
  and find_matching_dirs =
      (
        cwd,
        ~nested=false,
        ~get_workspace_patterns,
        ~read_parse_manifest,
        ~is_worktree,
        (),
      ) => {
    let cwd = cwd |> Fs.normalize_dir_path;

    let manifest = cwd |> read_parse_manifest;

    is_worktree(manifest)
      ? get_dirs(
          cwd,
          manifest,
          ~nested,
          ~get_workspace_patterns,
          ~read_parse_manifest,
          ~is_worktree,
          (),
        )
      : [Package(cwd)];
  };

  let rec find_root_dir =
          (
            path,
            ~read_parse_manifest,
            ~is_worktree,
            ~closest=None,
            ~depth=3,
            (),
          ) => {

    let stop = fun
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

    switch((hit_max_depth, hit_fs_root)) {
      | (true, _) => {
        Logs.debug(m => m("Stopping because we hit max depth without finding a worktree."));
        stop(closest)
      }
      | (_, true) => {
        Logs.debug(m => m("Stopping because we hit the FS root."));
        stop(closest)
      }
      | _ => {
        let dir_worktree =
          try(path |> read_parse_manifest |> is_worktree) {
          | _ => false
          };

        let frd =
          find_root_dir(
            path |> Fpath.parent,
            ~read_parse_manifest,
            ~is_worktree,
          );

        dir_worktree
          ? {
            Logs.debug(m =>
              m("Detected worktree at: %s", path |> Fpath.to_string)
            );
            frd(~closest=Some(path), ~depth=3, ());
          }
          : frd(~closest, ~depth=depth - 1, ());
      };

    }

   
  };
};

module Yarn = {
  [@deriving of_yojson({strict: false})]
  type t = {
    [@default None]
    workspaces: option(list(string)),
  };

  let manifest_file = "package.json";

  let path_to_manifest = path => manifest_file |> Fpath.add_seg(path);

  let read_manifest = path => path |> Fs.read_json;

  let parse_manifest = manifest => manifest |> of_yojson;

  let read_parse_manifest = path => {
    let path = path |> path_to_manifest;
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
  };

  let is_worktree = manifest =>
    switch (manifest.workspaces) {
    | Some(_) => true
    | None => false
    };

  let get_workspace_patterns = manifest =>
    manifest.workspaces
    |> (
      fun
      | Some(ws) => ws
      | None => []
    );

  let find_workspace_dirs = cwd =>
    Workspace.find_matching_dirs(
      cwd,
      ~get_workspace_patterns,
      ~read_parse_manifest,
      ~is_worktree,
      (),
    );

  let rec find_root_dir =
    Workspace.find_root_dir(~read_parse_manifest, ~is_worktree);

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
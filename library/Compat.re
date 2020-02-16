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

  let path_to_manifest = (path, manifest_file) =>
    manifest_file |> Fpath.append(path |> Fs.normalize_dir_path);

  let normalize_patterns = (cwd, patterns) =>
    patterns
    |> List.map(pattern => {
         let glob = Fpath.append(cwd, Fpath.v(pattern));
         switch (glob |> Fpath.to_dir_path |> Fpath.basename) {
         | "**" => glob
         | _ => Fpath.add_seg(glob, "package.json")
         };
       });

  let check_workspace_type =
      (path, ~manifest_file, ~read_parse_manifest, ~get_workspace_patterns) =>
    switch (manifest_file |> path_to_manifest(path) |> Fs.exists) {
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
    |> List.filter(s =>
         patterns
         |> List.exists(pattern => Glob.glob(pattern) |> Glob.match(~s))
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

  let max_package_depth = 5;

  let rec find_root_dir =
          (
            path,
            ~check_workspace_type,
            ~closest=None,
            ~depth=max_package_depth,
            (),
          ) => {
    let stop =
      fun
      | Some(dir) => {
          Logs.debug(m =>
            m("Found root directory of workspace: %s", dir |> Fpath.to_string)
          );
          Some(dir);
        }
      | None => None;

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
            frd(~closest=Some(p), ~depth=max_package_depth, ());
          }
        | _ => frd(~closest, ~depth=depth - 1, ())
      );
    };
  };
};

module Yarn = {
  open Protocol_conv_yaml;
  [@deriving protocol(~driver=(module Yaml))]
  type t = {
    [@default None]
    workspaces: option(list(string)),
  };

  let manifest_file = Fpath.v("package.json");

  let read_parse_manifest = Fs.read_and_parse(~parser=of_yaml);

  let get_workspace_patterns = manifest => manifest.workspaces;

  let check_workspace_type = path =>
    path
    |> Workspace.check_workspace_type(
         ~manifest_file,
         ~read_parse_manifest,
         ~get_workspace_patterns,
       );

  let find_workspace_dirs = cwd =>
    Workspace.find_matching_dirs(cwd, ~check_workspace_type, ());

  let rec find_root_dir = Workspace.find_root_dir(~check_workspace_type);

  let detect = cwd => {
    Logs.debug(m => m("Checking if this is a Yarn managed workspace..."));

    find_root_dir(cwd, ());
  };
};

module Pnpm = {
  open Protocol_conv_yaml;
  [@deriving protocol(~driver=(module Yaml))]
  type t = {
    [@default None]
    packages: option(list(string)),
  };

  let manifest_file = Fpath.v("pnpm-workspace.yaml");

  let read_parse_manifest = Fs.read_and_parse(~parser=of_yaml);

  let get_workspace_patterns = manifest => manifest.packages;

  let check_workspace_type = path =>
    path
    |> Workspace.check_workspace_type(
         ~manifest_file,
         ~read_parse_manifest,
         ~get_workspace_patterns,
       );

  let find_workspace_dirs = cwd =>
    Workspace.find_matching_dirs(cwd, ~check_workspace_type, ());

  let rec find_root_dir = Workspace.find_root_dir(~check_workspace_type);

  let detect = cwd => {
    Logs.debug(m => m("Checking if this is a Pnpm managed workspace..."));

    find_root_dir(cwd, ());
  };
};

module Rush = {
  open Protocol_conv_yaml;
  [@deriving protocol(~driver=(module Yaml))]
  type project = {
    packageName: string,
    projectFolder: string,
  };
  [@deriving protocol(~driver=(module Yaml))]
  type t = {projects: list(project)};

  let manifest_file = Fpath.v("rush.json");

  let read_parse_manifest = Fs.read_and_parse(~parser=of_yaml);

  let get_workspace_patterns = manifest =>
    Some(manifest.projects |> List.map(x => x.projectFolder));

  let check_workspace_type = path =>
    path
    |> Workspace.check_workspace_type(
         ~manifest_file,
         ~read_parse_manifest,
         ~get_workspace_patterns,
       );

  let find_workspace_dirs = cwd =>
    Workspace.find_matching_dirs(cwd, ~check_workspace_type, ());

  let rec find_root_dir = Workspace.find_root_dir(~check_workspace_type);

  let detect = cwd => {
    Logs.debug(m => m("Checking if this is a Rush managed workspace..."));

    find_root_dir(cwd, ());
  };
};

type t =
  | Rush
  | Pnpm
  | Yarn;

let to_string =
  fun
  | Rush => "Rush"
  | Yarn => "Yarn"
  | Pnpm => "Pnpm";

// This order matters since other tools might add workspaces support
// they need to be checked first so everything isn't set as Yarn
let managers = [
  (Pnpm.detect, Pnpm),
  (Rush.detect, Rush),
  (Yarn.detect, Yarn),
];

let detect_workspace_manager = cwd =>
  managers
  |> List.fold_left(
       (acc, (detect, kind)) => {
         switch (acc) {
         | Some(_) => acc
         | None =>
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
         }
       },
       None,
     );

let find_root_dir = (wsmgr, cwd) =>
  switch (wsmgr) {
  | Yarn => cwd |> Yarn.find_root_dir
  | Pnpm => cwd |> Pnpm.find_root_dir
  | Rush => cwd |> Rush.find_root_dir
  };

let find_workspace_dirs = (wsmgr, cwd) =>
  switch (wsmgr) {
  | Yarn => cwd |> Yarn.find_workspace_dirs
  | Pnpm => cwd |> Pnpm.find_workspace_dirs
  | Rush => cwd |> Rush.find_workspace_dirs
  };
module PackageJson = {
  open Protocol_conv_yaml;
  [@deriving protocol(~driver=(module Yaml))]
  type t = {
    name: string,
    [@default None]
    version: option(string),
  };

  let manifest_file = Fpath.v("package.json");

  let read_manifest = Fs.read_yaml;

  let parse_manifest = of_yaml;

  let read_parse_manifest = path => {
    path
    |> read_manifest
    |> parse_manifest
    |> (
      fun
      | Ok(wj) => wj
      | Error(err) => {
          raise(Errors.Fs_error(path, err |> Yaml.error_to_string_hum));
        }
    );
  };
};

module Workspace = {
  type kind = Compat.Workspace.t;

  type t = {
    kind,
    name: string,
    packageJson: PackageJson.t,
  };
};

let find_workspaces = cwd => {
  let cwd = cwd |> Fs.normalize_cwd;

  let (wsmgr, root) =
    cwd
    |> Compat.detect_workspace_manager
    |> (
      fun
      | Some((w, d)) => (w, d)
      | None => raise(Errors.No_workspace_found)
    );

  Compat.find_workspace_dirs(wsmgr, root)
  |> List.map((kind: Compat.Workspace.t) =>
       (
         {
           let path = kind |> Compat.Workspace.to_path;
           let packageJson =
             Compat.Workspace.path_to_manifest(
               path,
               PackageJson.manifest_file,
             )
             |> PackageJson.read_parse_manifest;

           {kind, name: packageJson.name, packageJson};
         }: Workspace.t
       )
     );
};
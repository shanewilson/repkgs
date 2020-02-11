module PackageJson = {
  open Protocol_conv_jsonm;
  [@deriving protocol(~driver=(module Jsonm))]
  type t = {
    name: string,
    [@default None]
    version: option(string),
  };

  let manifest_file = "package.json";

  let path_to_manifest = path => manifest_file |> Fpath.add_seg(path);

  let read_manifest = Fs.read_json;

  let parse_manifest = of_jsonm;

  let read_parse_manifest = path => {
    let path = path |> path_to_manifest;
    path
    |> read_manifest
    |> parse_manifest
    |> (
      fun
      | Ok(wj) => wj
      | Error(err) => {
          raise(
            Errors.Json_parse_error(path, err |> Jsonm.error_to_string_hum),
          );
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
           let packageJson = path |> PackageJson.read_parse_manifest;

           {kind, name: packageJson.name, packageJson};
         }: Workspace.t
       )
     );
};
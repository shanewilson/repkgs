open Re.Glob;

let match = (re, ~s) => Re.execp(Re.compile(re), s |> Fpath.to_string);

let glob = path => path |> Fpath.to_string |> Re.Glob.glob;
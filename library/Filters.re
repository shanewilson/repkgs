let contains = (s1, s2) => {
  let re = Str.regexp_string(s2);

  try(
    {
      ignore(Str.search_forward(re, s1, 0));
      true;
    }
  ) {
  | Not_found => false
  };
};

let include_worktree_filter = (ws, ~include_worktree) =>
  ws
  |> List.filter((pkg: Manager.Workspace.t) =>
       switch (include_worktree, pkg.kind) {
       | (true, _)
       | (_, Package(_)) => true
       | (false, _) => false
       }
     );

let since_filter = (ws, ~root, ~since) =>
  ws
  |> List.filter((w: Manager.Workspace.t) => {
       Git.git_tracked(root, ~since)
       |> List.exists(p =>
            contains(
              p |> Fpath.to_string,
              w.kind |> Compat.Workspace.to_path |> Fpath.to_string,
            )
          )
     });

let alpha_sort =
  List.fast_sort((a: Manager.Workspace.t, b: Manager.Workspace.t) =>
    String.compare(
      a.kind |> Compat.Workspace.to_path |> Fpath.to_string,
      b.kind |> Compat.Workspace.to_path |> Fpath.to_string,
    )
  );
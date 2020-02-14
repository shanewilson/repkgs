let include_worktree_filter = (ws, ~include_worktree) =>
  ws
  |> List.filter((pkg: Manager.Workspace.t) =>
       switch (include_worktree, pkg.kind) {
       | (true, _)
       | (_, Package(_)) => true
       | (false, _) => false
       }
     );

let alpha_sort =
  List.fast_sort((a: Manager.Workspace.t, b: Manager.Workspace.t) =>
    String.compare(
      a.kind |> Compat.Workspace.to_path |> Fpath.to_string,
      b.kind |> Compat.Workspace.to_path |> Fpath.to_string,
    )
  );
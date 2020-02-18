let contains = (s1, s2) => {
  let re = Str.regexp_string(s2 |> Fpath.to_string);

  try(
    {
      ignore(Str.search_forward(re, s1 |> Fpath.to_string, 0));
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

let alpha_sort =
  List.fast_sort((a: Manager.Workspace.t, b: Manager.Workspace.t) =>
    String.compare(
      a.kind |> Compat.Workspace.to_path |> Fpath.to_string,
      b.kind |> Compat.Workspace.to_path |> Fpath.to_string,
    )
  );
module Since = {
  let find_changed_packages = (ws, ~root, ~since) => {
    let tracked = Git.find_tracked_files(root, ~since);
    let changed =
      ws
      |> List.filter((w: Manager.Workspace.t) => {
           tracked
           |> List.exists(p =>
                contains(p, w.kind |> Compat.Workspace.to_path)
              )
         });

    Logs.info(m =>
      m("Found %i packages changed since: %s", changed |> List.length, since)
    );

    changed;
  };

  let since_filter = (ws, ~root, ~since, ~since_branch, ~since_latest_tag) =>
    switch (since, since_branch, since_latest_tag) {
    | ("", "", false) => ws
    | (_, _, true) =>
      let tag = Git.find_closest_tag(root);
      find_changed_packages(ws, ~root, ~since=tag);
    | (range, "", _) => find_changed_packages(ws, ~root, ~since=range)
    | (_, branch, _) =>
      find_changed_packages(ws, ~root, ~since=branch ++ "..")
    };
};

module Name = {
  let include_matching_packages = (ws, ~patterns) =>
    switch (patterns) {
    | [] => ws
    | ps =>
      let matches =
        ws
        |> List.filter((w: Manager.Workspace.t) =>
             ps |> Glob.String.match_patterns(~s=w.name)
           );

      Logs.info(m =>
        m("Found %i packages matching patterns", matches |> List.length)
      );

      matches;
    };

  let exclude_matching_packages = (ws, ~patterns) =>
    switch (patterns) {
    | [] => ws
    | ps =>
      let matches =
        ws
        |> List.filter((w: Manager.Workspace.t) =>
             !(patterns |> Glob.String.match_patterns(~s=w.name))
           );

      Logs.info(m =>
        m("Ignoring %i packages matching patterns", matches |> List.length)
      );

      matches;
    };
};
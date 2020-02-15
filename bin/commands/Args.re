let cwd = {
  let doc = "Run the program in directory $(docv).";
  Cmdliner.Arg.(
    value
    & opt(dir, Library.Fs.get_cwd() |> Fpath.to_string)
    & info(["cwd"], ~docv="DIR", ~doc)
  );
};

module Since = {
  let docs = "CHANGED SINCE FILTERS";

  let since = {
    let doc = "Find packages modified since $(docv)";
    Cmdliner.Arg.(
      value
      & opt(~vopt="HEAD..", string, "")
      & info(["since"], ~docv="COMMIT", ~doc, ~docs)
    );
  };

  let since_branch = {
    let doc = "Find packages modified since $(docv). Takes over $(b,--since).";
    Cmdliner.Arg.(
      value
      & opt(~vopt="master", string, "")
      & info(["since-branch"], ~docv="BRANCH/TAG", ~doc, ~docs)
    );
  };

  let since_latest_tag = {
    let doc = "Find packages modified since the closest annotated tag. Takes over $(b,--since) and $(b,--since-branch).";
    Cmdliner.Arg.(value & flag & info(["since-latest-tag"], ~doc, ~docs));
  };
};

let include_worktree = {
  let doc = "Include worktree/root dir.";
  Cmdliner.Arg.(value & flag & info(["include-worktree"], ~doc));
};
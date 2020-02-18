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

module Name = {
  let docs = "PACKAGE NAME FILTERS";

  let included = {
    let doc = "Include all packages whose name matches pattern $(docv)";
    Cmdliner.Arg.(
      value
      & opt_all(string, [])
      & info(["include"], ~docv="GLOB", ~doc, ~docs)
    );
  };

  let excluded = {
    let doc = "Exclude any package whose name matches pattern $(docv)";
    Cmdliner.Arg.(
      value
      & opt_all(string, [])
      & info(["exclude"], ~docv="GLOB", ~doc, ~docs)
    );
  };
};
module Fs = {
  let docs = "FILE SYSTEM FILTERS";

  let only = {
    let doc = "Include all packages with any files matching pattern $(docv)";
    Cmdliner.Arg.(
      value
      & opt_all(string, [])
      & info(["only-fs"], ~docv="GLOB", ~doc, ~docs)
    );
  };

  let ignore = {
    let doc = "Exclude any package with any files matching pattern $(docv)";
    Cmdliner.Arg.(
      value
      & opt_all(string, [])
      & info(["ignore-fs"], ~docv="GLOB", ~doc, ~docs)
    );
  };
};

module Include = {
  let include_worktree = {
    let doc = "Include worktree/root dir.";
    Cmdliner.Arg.(value & flag & info(["include-worktree"], ~doc));
  };

  let include_private = {
    let doc = "Include private packages.";
    Cmdliner.Arg.(value & flag & info(["include-private"], ~doc));
  };
};
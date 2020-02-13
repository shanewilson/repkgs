let cwd = {
  let doc = "Run the program in directory $(docv).";
  Cmdliner.Arg.(
    value
    & opt(dir, Library.Fs.get_cwd() |> Fpath.to_string)
    & info(["cwd"], ~docv="DIR", ~doc)
  );
};

let include_worktree = {
  let doc = "Include worktree/root dir.";
  Cmdliner.Arg.(value & flag & info(["include-worktree"], ~doc));
};
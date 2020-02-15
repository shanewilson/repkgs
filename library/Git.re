open Rresult;

let git = Cmd.v("git");

let find_first_commit = root => {
  let first_commit = Cmd.(git % "rev-list" % "--max-parents=0" % "HEAD");

  OS.Dir.with_current(root, OS.Cmd.out_string, first_commit |> OS.Cmd.run_out)
  >>= (r => r)
  |> (
    fun
    | Ok((output, _)) => {
        Logs.info(m => m("Found root commit: %s", output));
        output;
      }
    | Error(`Msg(msg)) => failwith(msg)
  );
};

let find_closest_tag = root => {
  let closest_tag = Cmd.(git % "describe" % "--abbrev=0");

  OS.Dir.with_current(root, OS.Cmd.out_string, closest_tag |> OS.Cmd.run_out)
  >>= (r => r)
  |> (
    fun
    | Ok((output, (run_info, status))) => {
        switch (status) {
        | `Exited(int) =>
          Logs.info(m => m("Could not find an annotated git tag"));
          find_first_commit(root);
        | _ =>
          Logs.info(m => m("Found git tag: %s", output));
          output;
        };
      }
    | Error(`Msg(msg)) => failwith(msg)
  );
};

let find_tracked_files = (root, ~since) => {
  let ls_files = Cmd.(git % "diff" % "--name-only" % "--relative" % since);

  OS.Dir.with_current(root, OS.Cmd.to_lines, ls_files |> OS.Cmd.run_out)
  >>= (r => r)
  |> (
    fun
    | Ok(r) => r |> List.map(p => Fpath.append(root, Fpath.v(p)))
    | Error(`Msg(msg)) => failwith(msg)
  );
};
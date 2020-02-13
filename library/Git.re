open Rresult;

let git = Cmd.v("git");

let git_tracked = (root, ~since) => {
  let git_ls_files = Cmd.(git % "diff" % "--name-only" % "--relative" % since);

  OS.Dir.with_current(root, OS.Cmd.to_lines, git_ls_files |> OS.Cmd.run_out)
  >>= (r => r)
  |> (
    fun
    | Ok(r) => r |> List.map(p => Fpath.append(root, Fpath.v(p)))
    | Error(`Msg(msg)) => failwith(msg)
  );
};
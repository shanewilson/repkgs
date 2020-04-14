let cmd = (args, ~cwd) =>
  CrossSpawn.sync(
    "git",
    args,
    Node.Child_process.option(~encoding="utf8", ~cwd=cwd->Path.toString, ()),
  )
  ->Node.Child_process.readAs##stdout
  ->Js.Null.toOption
  // How do you deal with Node.string_buffer?!
  ->(
      fun
      | None => "maybe make this a Result"
      | Some(s) => {j|$s|j}
    );
let findFirstCommit = (~cwd) =>
  cmd([|"rev-list", "--max-parents=0", "HEAD"|], ~cwd);
let findClosestTag = (~cwd) => cmd([|"describe", "--abbrev=0"|], ~cwd);
let findTrackedFiles = (since, ~cwd) =>
  cmd([|"diff", "--name-only", since, "--relative"|], ~cwd)
  ->Js.String2.split(NodeOS.eol)
  ->Array.keep(s => s->String.length > 0)
  ->List.fromArray;
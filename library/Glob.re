open Re.Glob;

let match = (re, ~s) => Re.execp(Re.compile(re), s |> Fpath.to_string);

let glob = path => path |> Fpath.to_string |> Re.Glob.glob;

let create_globs_seq = pattern => {
  pattern
  |> Fpath.segs
  |> List.fold_left(
       ((patterns, glob), x) => {
         switch (x) {
         | "**" => (
             [
               Re.Glob.glob("/**/", ~pathname=false),
               Re.Glob.glob(glob, ~pathname=true),
               ...patterns,
             ],
             "",
           )
         | _ => (
             patterns,
             switch (glob) {
             | "" => x
             | _ => Fpath.add_seg(Fpath.v(glob), x) |> Fpath.to_string
             },
           )
         }
       },
       ([], ""),
     )
  |> (
    ((patterns, glob)) =>
      switch (glob) {
      | "" => patterns
      | _ => [Re.Glob.glob(glob, ~pathname=true), ...patterns]
      }
  )
  |> List.rev;
};

let match_patterns = (~path) =>
  List.exists(pattern =>
    Re.execp(Re.seq(create_globs_seq(pattern)) |> Re.compile, path)
  );

let matches = (~patterns) =>
  List.filter(path => patterns |> match_patterns(~path));
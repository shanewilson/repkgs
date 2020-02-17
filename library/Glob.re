open Re.Glob;

let match = (re, ~s) => Re.execp(Re.compile(re), s |> Fpath.to_string);

let glob = path => path |> Fpath.to_string |> Re.Glob.glob;

let create_globs_seq = pattern => {
  pattern
  |> Fpath.segs
  |> List.fold_left(
       ((patterns, glob), x) => {
         let patterns =
           switch (x) {
           | "**" => [
               Re.Glob.glob("/**/", ~pathname=false),
               Re.Glob.glob(glob, ~pathname=true),
               ...patterns,
             ]
           | _ => patterns
           };
         let glob =
           switch (glob) {
           | "" => x
           | _ => Fpath.add_seg(Fpath.v(glob), x) |> Fpath.to_string
           };
         (patterns, glob);
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
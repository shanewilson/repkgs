let glob =
    (~anchored=false, ~pathname=true, ~period=false, ~expand_braces=true) =>
  Re.Glob.glob(~anchored, ~pathname, ~period, ~expand_braces);

module String = {
  let match = (pattern, ~s) =>
    Re.execp(
      glob(pattern, ~anchored=true, ~pathname=false) |> Re.compile,
      s,
    );

  let match_patterns = (~s) => List.exists(match(~s));

  let matches = (~patterns) =>
    List.filter(s => patterns |> match_patterns(~s));
};

module Path = {
  let create_globs_seq = pattern => {
    pattern
    |> Fpath.v
    |> Fpath.segs
    |> List.fold_left(
         ((patterns, temp), x) => {
           switch (x) {
           | "**" => (
               [
                 glob("/**", ~pathname=false),
                 glob(temp, ~pathname=true),
                 ...patterns,
               ],
               "",
             )
           | _ => (
               patterns,
               switch (temp) {
               | "" => x
               | _ => Fpath.add_seg(Fpath.v(temp), x) |> Fpath.to_string
               },
             )
           }
         },
         ([], ""),
       )
    |> (
      ((patterns, temp)) =>
        switch (temp) {
        | "" => patterns
        | _ => [glob(temp, ~pathname=true), ...patterns]
        }
    )
    |> List.rev;
  };

  let match = (pattern, ~s) =>
    Re.execp(
      Re.seq(create_globs_seq(pattern)) |> Re.compile,
      s |> Fpath.to_string,
    );

  let match_patterns = (~s) => List.exists(match(~s));

  let matches = (~patterns) =>
    List.filter(s => patterns |> match_patterns(~s));
};
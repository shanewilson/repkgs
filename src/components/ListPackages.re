include Ink;

[@react.component]
let make =
    (
      ~cwd,
      ~workspaces,
      ~includePrivate,
      ~nameOnly,
      ~nameIgnored,
      ~pathOnly,
      ~pathIgnored,
      ~fsOnly,
      ~fsIgnored,
      ~since,
      ~sinceBranch,
      ~sinceLatestTag,
    ) => {
  let root = cwd->Workspace.findRoot;
  let workspace = root->Workspace.v(~workspaces);

  switch (workspace) {
  | Error(error) => <Error error />
  | Ok(wk) =>
    let packages =
      wk
      ->Workspace.packages
      ->Filters.make(
          ~cwd=root,
          ~includePrivate,
          ~nameOnly,
          ~nameIgnored,
          ~pathOnly,
          ~pathIgnored,
          ~fsOnly,
          ~fsIgnored,
          ~since,
          ~sinceBranch,
          ~sinceLatestTag,
        )
      ->List.sort((a, b) =>
          String.compare(
            a->Package.path->Path.toString,
            b->Package.path->Path.toString,
          )
        );
    <Box flexDirection="column">
      <Color whiteBright=true>
        "\nFound "->React.string
        <Color bold=true>
          {packages->List.length->Int.toString->React.string}
        </Color>
        " packages\n\n"->React.string
      </Color>
      {packages
       ->List.map(p =>
           <Box key={p->Package.name}>
             <Color> {p->Package.name->React.string} </Color>
             {switch (p) {
              | Private(_) =>
                <Color blackBright=true italic=true>
                  " (private)"->React.string
                </Color>
              | _ => React.null
              }}
           </Box>
         )
       ->List.toArray
       ->ReasonReact.array}
    </Box>;
  };
};
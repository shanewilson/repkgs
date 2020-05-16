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
    let verifiedPackages =
      packages
      ->List.keep(
          fun
          | Publishable(_) => true
          | _ => false,
        )
      ->List.map(VerifiedPackage.v)
      ->List.keep(x => x.errors > 0);

    <Box flexDirection="column">
      <Color whiteBright=true>
        "\nFound "->React.string
        <Color bold=true>
          {verifiedPackages->List.length->Int.toString->React.string}
        </Color>
        " packages with errors\n\n"->React.string
      </Color>
      {verifiedPackages
       ->List.map(vp => {
           switch (vp.errors) {
           | 0 => React.null
           | _ =>
             <Box
               key={vp.pkg->Package.name}
               flexDirection="column"
               marginBottom=1>
               <Box>
                 {js|📦  |js}->React.string
                 <Color white=true>
                   {vp.pkg->Package.name->React.string}
                 </Color>
               </Box>
               <Border />
               <Box flexDirection="column">
                 <VerifyFilesMatchPaths name="bin:" field={vp.bin} />
                 <VerifyFilesMatchPaths name="main:" field={vp.main} />
                 <VerifyFilesMatchPaths name="types:" field={vp.types} />
                 <VerifyFilesMatchPaths
                   name="packed files:"
                   field={vp.files}
                 />
                 <VerifyFilesMissingImports
                   name="required files not in pack:"
                   required={vp.packed.imports->ImportSet.keepLocalImports}
                   found={vp.packed.files}
                   path={vp.pkg->Package.path}
                 />
               </Box>
             </Box>
           }
         })
       ->List.toArray
       ->ReasonReact.array}
    </Box>;
  };
};
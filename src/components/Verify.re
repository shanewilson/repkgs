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
      ->List.keep(x => x.missingPaths.errors + x.imports.errors > 0);

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
           switch (vp.missingPaths.errors + vp.imports.errors) {
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
               <Box flexDirection="column">
                 {switch (vp.missingPaths.errors) {
                  | 0 => React.null
                  | _ =>
                    <Box flexDirection="column">
                      <Border />
                      <Border>
                        <Color cyan=true bold=true>
                          "Missing local files:"->React.string
                        </Color>
                      </Border>
                      <VerifyPathsSection
                        name="bin:"
                        field={vp.missingPaths.bin}
                      />
                      <VerifyPathsSection
                        name="main:"
                        field={vp.missingPaths.main}
                      />
                      <VerifyPathsSection
                        name="types:"
                        field={vp.missingPaths.types}
                      />
                      <VerifyPathsSection
                        name="files:"
                        field={vp.missingPaths.files}
                      />
                      <VerifyImportsSection
                        name="required files not in pack:"
                        imports={vp.imports.missingLocalImports}
                        path={vp.pkg->Package.path}
                      />
                    </Box>
                  }}
                 {switch (vp.imports.errors) {
                  | 0 => React.null
                  | _ =>
                    <Box flexDirection="column">
                      <Border />
                      <Border>
                        <Color cyan=true bold=true>
                          "Dependency checks:"->React.string
                        </Color>
                      </Border>
                      <VerifyImportsSection
                        name="required dependencies missing:"
                        imports={vp.imports.missingExternalImports}
                        path={vp.pkg->Package.path}
                      />
                      <VerifyImportsSection
                        name="unused dependencies:"
                        imports={vp.imports.unusedExternalImports}
                        path={vp.pkg->Package.path}
                      />
                    </Box>
                  }}
               </Box>
             </Box>
           }
         })
       ->List.toArray
       ->ReasonReact.array}
    </Box>;
  };
};
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
      {packages
       ->List.map(p =>
           switch (
             p->Package.packageJson->PackageJson.main,
             p->Package.packageJson->PackageJson.bin,
             p->Package.packageJson->PackageJson.files,
           ) {
           | (None, None, ["*"]) => React.null
           | _ =>
             <Box key={p->Package.name} flexDirection="column" marginBottom=1>
               <Box>
                 <Color white=true> {p->Package.name->React.string} </Color>
               </Box>
               <Border />
               {switch (p) {
                | Publishable(pkg) =>
                  <Box flexDirection="column">
                    {switch (pkg.packageJson->PackageJson.main) {
                     | Some(f) =>
                       <Border>
                         <Color cyan=true bold=true>
                           "main: "->React.string
                         </Color>
                         <Color white=true>
                           {(f ++ " ..... ")->React.string}
                         </Color>
                         {switch (pkg.path->Path.addSeg(f)->Fs.exists) {
                          | Ok(_) =>
                            <Color green=true> "found"->React.string </Color>
                          | Error(_) =>
                            <Color red=true> "missing"->React.string </Color>
                          }}
                       </Border>
                     | None => React.null
                     }}
                    {switch (pkg.packageJson->PackageJson.bin) {
                     | Some(arr) =>
                       <Box flexDirection="column">
                         <Border>
                           <Color cyan=true bold=true>
                             "bin:"->React.string
                           </Color>
                         </Border>
                         {arr
                          ->List.toArray
                          ->Array.map(f =>
                              <Border key=f>
                                <Color white=true>
                                  {("\t" ++ f ++ " .....  ")->React.string}
                                </Color>
                                {switch (pkg.path->Path.addSeg(f)->Fs.exists) {
                                 | Ok(_) =>
                                   <Color green=true>
                                     "found"->React.string
                                   </Color>
                                 | Error(_) =>
                                   <Color red=true>
                                     "missing"->React.string
                                   </Color>
                                 }}
                              </Border>
                            )
                          ->React.array}
                       </Box>
                     | None => React.null
                     }}
                    {switch (pkg.packageJson->PackageJson.files) {
                     | ["*"] => React.null
                     | arr =>
                       <Box flexDirection="column">
                         <Border>
                           <Color cyan=true bold=true>
                             "packed files:"->React.string
                           </Color>
                         </Border>
                         {arr
                          ->List.toArray
                          ->Array.map(fpat =>
                              <Border key=fpat>
                                <Color white=true>
                                  {("\t" ++ fpat ++ " .....  ")->React.string}
                                </Color>
                                {switch (
                                   pkg.path->Path.addSeg(fpat)->Fs.exists
                                 ) {
                                 | Ok(_) =>
                                   <Color green=true>
                                     "found"->React.string
                                   </Color>
                                 | Error(_) =>
                                   switch (
                                     [fpat]
                                     ->Glob.v(~cwd=pkg.path)
                                     ->Glob.vmatch
                                   ) {
                                   | [] =>
                                     <Color red=true>
                                       "no matches"->React.string
                                     </Color>
                                   | xs =>
                                     <Color green=true>
                                       {(
                                          "found "
                                          ++ xs->List.length->Int.toString
                                          ++ " matches"
                                        )
                                        ->React.string}
                                     </Color>
                                   }
                                 }}
                              </Border>
                            )
                          ->React.array}
                       </Box>
                     }}
                    {let files = p->Pack.gatherFilesFromJson;
                     let imports = files->Pack.findImports;
                     switch (
                       Set.String.diff(
                         imports
                         ->List.keep(
                             fun
                             | Local(_) => true
                             | _ => false,
                           )
                         ->List.map(Pack.pp(~cwd=p->Package.path))
                         ->List.toArray
                         ->Set.String.fromArray,
                         files
                         ->List.map(Pack.pp(~cwd=p->Package.path))
                         ->List.toArray
                         ->Set.String.fromArray,
                       )
                       ->Set.String.toArray
                     ) {
                     | [||] => React.null
                     | arr =>
                       <Box flexDirection="column">
                         <Border>
                           <Color cyan=true bold=true>
                             "required files not in pack:"->React.string
                           </Color>
                         </Border>
                         {arr
                          ->Array.map(a =>
                              <Border key=a>
                                <Color red=true>
                                  {("\t" ++ a)->React.string}
                                </Color>
                              </Border>
                            )
                          ->React.array}
                       </Box>
                     }}
                    {let files = p->Pack.gatherFilesFromJson;
                     let imports = files->Pack.findImports;
                     switch (
                       Set.String.diff(
                         imports
                         ->List.keep(
                             fun
                             | Unresolved(_) => true
                             | _ => false,
                           )
                         ->List.map(Pack.pp(~cwd=p->Package.path))
                         ->List.toArray
                         ->Set.String.fromArray,
                         files
                         ->List.map(Pack.pp(~cwd=p->Package.path))
                         ->List.toArray
                         ->Set.String.fromArray,
                       )
                       ->Set.String.toArray
                     ) {
                     | [||] => React.null
                     | arr =>
                       <Box flexDirection="column">
                         <Border>
                           <Color cyan=true bold=true>
                             "broken imports:"->React.string
                           </Color>
                         </Border>
                         {arr
                          ->Array.map(a =>
                              <Border key=a>
                                <Color red=true>
                                  {("\t" ++ a)->React.string}
                                </Color>
                              </Border>
                            )
                          ->React.array}
                       </Box>
                     }}
                  </Box>
                | _ => React.null
                }}
             </Box>
           }
         )
       ->List.toArray
       ->ReasonReact.array}
    </Box>;
  };
};
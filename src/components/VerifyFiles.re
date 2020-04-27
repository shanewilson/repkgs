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
                             "files:"->React.string
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
                    <Box flexDirection="column">
                      "\n"->React.string
                      {[
                         p
                         ->Package.packageJson
                         ->PackageJson.main
                         ->Option.flatMap(s => Some([s])),
                         p->Package.packageJson->PackageJson.bin,
                         Some(p->Package.packageJson->PackageJson.files),
                       ]
                       ->List.reduce([], (acc, xs) =>
                           switch (xs) {
                           | Some(xs) => acc->List.concat(xs)
                           | None => acc
                           }
                         )
                       ->List.map(s => {
                           let x = p->Package.path->Path.addSeg(s);
                           switch (x->Fs.v) {
                           | File(file) => [file]
                           | Dir(dir) =>
                             [dir->Path.addSeg("**")->Path.toString]
                             ->Glob.v(~cwd=p->Package.path)
                             ->Glob.vmatch
                           | DNE(_) =>
                             [s]->Glob.v(~cwd=p->Package.path)->Glob.vmatch
                           };
                         })
                       ->List.flatten
                       ->List.map(s =>
                           "\n"
                           ++ s->Path.pp
                           ++ "\n\n"
                           ++ s
                              ->Fs.read
                              ->Result.getExn
                              ->FlowParser.parse
                              ->Js.Json.stringify
                         )
                       ->List.map(React.string)
                       ->List.toArray
                       ->React.array}
                    </Box>
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
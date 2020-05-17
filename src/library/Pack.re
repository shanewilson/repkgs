let gatherFilesFromJson = pkg => {
  let path = pkg->Package.path;
  let pjson = pkg->Package.packageJson;

  [
    pjson->PackageJson.bin,
    pjson->PackageJson.main->Option.flatMap(s => Some([s])),
    Some(pjson->PackageJson.files),
  ]
  ->List.reduce([], (acc, oxs) => {
      switch (oxs) {
      | Some(xs) => acc->List.concat(xs)
      | None => acc
      }
    })
  ->List.map(s => {
      switch (path->Path.addSeg(s)->Fs.v) {
      | File(_) => s
      // 1. can I use / for this?
      // 2. This is just until I figure out a nice way
      // to deal with src files like jsx/tsx/ts
      // I want it to make sense rather than just
      // be a shotgun approach of file exists
      | Dir(_) => s ++ "/**/*.js"
      | DNE(_) =>
        switch (s) {
        | "*" => "**/*.js"
        | pat => pat
        }
      }
    })
  ->Glob.v(~cwd=path)
  ->Glob.vmatch
  ->List.keep(p => p->Path.toString->Js.String2.endsWith(".js"))
  ->List.map(x => ImportSet.Import.v(x->Path.toString, ~path))
  ->ImportSet.fromList;
};
let parseImports = paths =>
  paths
  ->List.map(t => {
      switch (t) {
      | ImportSet.Import.Local(t) =>
        switch (t.target->Fs.read) {
        | Ok(s) =>
          // obviously slow on big files
          // should make custom require/import
          // parser that ignores everything else
          switch (s->FlowParser.parse) {
          | Ok(json) =>
            json->AST.programDecode.body
            ->List.fromArray
            ->List.map(x => {
                Js.log(x);
                Some(x)->AST.parseRequires;
              })
            ->List.flatten
            ->List.map(x => x->ImportSet.Import.v(~path=t.target))
          | Error(_) => []
          }
        | Error(_) => []
        }
      | notPath => [notPath]
      }
    })
  ->List.flatten;
let findImports = paths => {
  let r = paths->ImportSet.toList->parseImports;

  r
  ->(
      x =>
        switch (x) {
        | [] => []
        | xs => xs->List.concat(xs->parseImports)
        }
    )
  ->ImportSet.fromList;
};

let filesInPack = x => x;
let filesRequiredByPack = x => x;
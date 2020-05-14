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
      // can I use / for this?
      | Dir(_) => s ++ "/**"
      | DNE(_) =>
        switch (s) {
        | "*" => "**"
        | pat => pat
        }
      }
    })
  ->Glob.v(~cwd=path)
  ->Glob.vmatch
  ->List.keep(p
      // This is just until I figure out a nice way to deal with src files like jsx/tsx/ts
      // I want it to make sense rather than just be a shotgun approach of file exts
      => p->Path.toString->Js.String2.endsWith(".js"))
  ->List.map(x => ImportSet.Import.v(x->Path.toString, ~path))
  ->ImportSet.fromList;
};
let parseImports = paths =>
  paths
  ->List.map(t => {
      switch (t) {
      | ImportSet.Import.Local(p) =>
        switch (p->Fs.read) {
        | Ok(s) =>
          switch (s->FlowParser.parse) {
          | Ok(json) =>
            json->AST.decode.body
            ->List.fromArray
            ->List.map(x => Some(x)->AST.parseRequires)
            ->List.flatten
            ->List.map(x => x->ImportSet.Import.v(~path=p))
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
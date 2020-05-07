type t =
  | Local(Path.t)
  | Unresolved(Path.t)
  | External(string);

let v = (import, ~path) =>
  import->Js.String2.startsWith(".")
    ? {
      let filepath = path->Path.parent->Path.addSeg(import);
      filepath
      ->RequireLocal.resolve
      ->(
          fun
          | Ok(p) => Local(p)
          | _ => Unresolved(filepath)
        );
    }
    : External(import);

let toString =
  fun
  | Local(p)
  | Unresolved(p) => p->Path.toString
  | External(s) => s;

let gatherFilesFromJson = (pkg: Package.t): List.t(t) => {
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
  ->List.map(x => Local(x));
};

let parseImports = paths =>
  paths
  ->List.map(t => {
      switch (t) {
      | Local(p) =>
        switch (p->Fs.read) {
        | Ok(s) =>
          switch (s->FlowParser.parse) {
          | Ok(json) =>
            json->AST.decode.body
            ->List.fromArray
            ->List.map(x => Some(x)->AST.parseRequires)
            ->List.flatten
            ->List.map(x => x->v(~path=p))
          | Error(_) => []
          }
        | Error(_) => []
        }
      | notPath => [notPath]
      }
    })
  ->List.flatten;
// ->List.flatten

let findImports = paths => {
  let r = paths->parseImports;

  r->(
       x =>
         switch (x) {
         | [] => []
         | xs => xs->List.concat(xs->parseImports)
         }
     );
};

let keepLocalImports = List.keep(_, s => s->Js.String2.startsWith("."));
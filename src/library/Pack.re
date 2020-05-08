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

let pp = (x, ~cwd) => x->toString->Path.v->Path.relativize(~cwd)->Path.pp;

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
  ->List.keep(p
      // This is just until I figure out a nice way to deal with src files like jsx/tsx/ts
      // I want it to make sense rather than just be a shotgun approach of file exts
      => p->Path.toString->Js.String2.endsWith(".js"))
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
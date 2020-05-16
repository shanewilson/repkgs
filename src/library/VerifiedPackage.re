type paths = {
  main: List.t(string),
  types: List.t(string),
  bin: List.t(string),
  files: List.t(string),
};
type imports = {
  missingLocalImports: ImportSet.t,
  missingExternalImports: ImportSet.t,
  unusedExternalImports: ImportSet.t,
  brokenImports: ImportSet.t,
};
type inPack = {
  files: ImportSet.t,
  imports: ImportSet.t,
};
type packed = inPack;
type t = {
  pkg: Package.t,
  paths,
  imports,
  inPack,
  main: List.t(string),
  types: List.t(string),
  bin: List.t(string),
  files: List.t(string),
  errors: int,
  packed,
};
let findMissingPaths = (ls, ~path) =>
  ls->List.keep(_, f =>
        switch (path->Path.addSeg(f)->Fs.exists) {
        | Ok(_) => false
        | Error(_) =>
          switch ([f]->Glob.v(~cwd=path)->Glob.vmatch) {
          | [] => true
          | _ => false
          }
        }
      );
let getOpt = (f, ~path) =>
  switch (f) {
  | Some(x) => x->findMissingPaths(~path)
  | None => []
  };
let v = (pkg: Package.t): t => {
  let path = pkg->Package.path;
  let json = pkg->Package.packageJson;
  let main = json->PackageJson.main->Option.map(x => [x])->getOpt(~path);
  let types = json->PackageJson.types->Option.map(x => [x])->getOpt(~path);
  let bin = json->PackageJson.bin->getOpt(~path);
  let files = Some(json->PackageJson.files)->getOpt(~path);

  let inPack =
    switch (pkg->Package.packageJson->PackageJson.files) {
    | ["*"] => {files: ImportSet.empty, imports: ImportSet.empty}
    | _ =>
      let fs = pkg->Pack.gatherFilesFromJson;
      Js.log(fs->ImportSet.toArray);
      Js.log(fs->Pack.findImports->ImportSet.toArray);
      {files: fs, imports: fs->Pack.findImports};
    };

  {
    pkg,
    inPack,
    paths: {
      main,
      types,
      bin,
      files,
    },
    imports: {
      missingLocalImports: ImportSet.empty,
      missingExternalImports: ImportSet.empty,
      unusedExternalImports: ImportSet.empty,
      brokenImports: ImportSet.empty,
    },
    main,
    types,
    bin,
    files,
    errors:
      main->List.length
      + types->List.length
      + bin->List.length
      + files->List.length,
    packed: inPack,
  };
};
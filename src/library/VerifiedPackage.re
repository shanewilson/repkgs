type missingPaths = {
  main: List.t(string),
  types: List.t(string),
  bin: List.t(string),
  files: List.t(string),
  errors: int,
};
type imports = {
  missingLocalImports: ImportSet.t,
  missingExternalImports: ImportSet.t,
  unusedExternalImports: ImportSet.t,
  brokenLocalImports: ImportSet.t,
  errors: int,
};
type inPack = {
  files: ImportSet.t,
  imports: ImportSet.t,
};
type t = {
  pkg: Package.t,
  missingPaths,
  imports,
  inPack,
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

  let inPack = {
    let fs = pkg->Pack.gatherFilesFromJson;
    {files: fs, imports: fs->Pack.findImports};
  };

  let missingLocalImports =
    ImportSet.diff(inPack.imports->ImportSet.keepLocalImports, inPack.files);
  let brokenLocalImports =
    ImportSet.diff(inPack.imports->ImportSet.keepBrokenImports, inPack.files);
  let missingExternalImports =
    ImportSet.diff(
      inPack.imports->ImportSet.keepExternalImports,
      pkg->Package.dependencies,
    );
  let unusedExternalImports =
    ImportSet.diff(
      pkg->Package.dependencies,
      inPack.imports->ImportSet.keepExternalImports,
    );

  {
    pkg,
    inPack,
    missingPaths: {
      main,
      types,
      bin,
      files,
      errors:
        main->List.length
        + types->List.length
        + bin->List.length
        + files->List.length,
    },
    imports: {
      missingLocalImports,
      missingExternalImports,
      unusedExternalImports,
      brokenLocalImports,
      errors:
        + missingLocalImports->ImportSet.size
        + missingExternalImports->ImportSet.size
        + unusedExternalImports->ImportSet.size
        + brokenLocalImports->ImportSet.size,
    },
  };
};
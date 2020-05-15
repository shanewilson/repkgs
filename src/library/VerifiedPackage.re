type packed = {
  files: ImportSet.t,
  imports: ImportSet.t,
};
type t = {
  pkg: Package.t,
  main: List.t(string),
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
let v = (pkg: Package.t): t => {
  let path = pkg->Package.path;
  let main =
    switch (pkg->Package.packageJson->PackageJson.main) {
    | Some(x) => [x]->findMissingPaths(~path)
    | None => []
    };
  let bin =
    switch (pkg->Package.packageJson->PackageJson.bin) {
    | Some(xs) => xs->findMissingPaths(~path)
    | None => []
    };
  let files =
    pkg->Package.packageJson->PackageJson.files->findMissingPaths(~path);
  let packed: packed =
    switch (pkg->Package.packageJson->PackageJson.files) {
    | ["*"] => {files: ImportSet.empty, imports: ImportSet.empty}
    | _ =>
      let fs = pkg->Pack.gatherFilesFromJson;
      {files: fs, imports: fs->Pack.findImports};
    };
  {
    pkg,
    main,
    bin,
    files,
    errors: main->List.length + bin->List.length + files->List.length,
    packed,
  };
};
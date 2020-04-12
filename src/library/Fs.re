type stat = {
  .
  [@bs.meth] isFile: unit => bool,
  [@bs.meth] isDirectory: unit => bool,
  isCharacterDevice: unit => bool,
  isSymbolicLink: unit => bool,
  isFIFO: unit => bool,
  isSocket: unit => bool,
};
[@bs.module "fs"] external lstatSync: string => Js.t(stat) = "lstatSync";

module Error = {
  type t = [
    | `FsDoesNotExist(Path.t)
    | `FsNotAFile(Path.t)
    | `FsNotADirectory(Path.t)
  ];
};

type t =
  | Dir(Path.t)
  | File(Path.t)
  | DNE(Path.t);
let toPath =
  fun
  | Dir(p)
  | File(p)
  | DNE(p) => p;
let toString = p => p->toPath->Path.toString;
let pp = p => p->toPath->Path.pp;
let exists = p =>
  p->Path.toString->Node.Fs.existsSync ? p->Ok : `FsDoesNotExist(p)->Error;
let isDirectory = p =>
  lstatSync(p->Path.toString)##isDirectory()
    ? p->Ok : `FsNotADirectory(p)->Error;
let isFile = p =>
  lstatSync(p->Path.toString)##isFile() ? p->Ok : `FsNotAFile(p)->Error;
let v = p =>
  switch (p->exists) {
  | Ok(p) =>
    switch (p->isDirectory) {
    | Ok(p) => Dir(p)
    | _ => File(p)
    }
  | _ => DNE(p)
  };
let toDirectory = p =>
  switch (p->v) {
  | Dir(p) => p->Ok
  | File(p) => p->Path.parent->Ok
  | DNE(p) => `FsDoesNotExist(p)->Error
  };
let read = p =>
  switch (p->v) {
  | File(p) =>
    try((p->Path.toString ++ "")->Node.Fs.readFileAsUtf8Sync->Ok) {
    | _ => `FsDoesNotExist(p)->Error
    }
  | Dir(_) => `FsNotAFile(p)->Error
  | DNE(_) => `FsDoesNotExist(p)->Error
  };
let contents = p =>
  switch (p->v) {
  | Dir(p) => p->Path.toString->Node.Fs.readFileAsUtf8Sync->Ok
  | File(_) => `FsNotADirectory(p)->Error
  | DNE(_) => `FsDoesNotExist(p)->Error
  };
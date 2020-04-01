type stat = {
  .
  isFile: unit => bool,
  [@bs.meth] isDirectory: unit => bool,
  isCharacterDevice: unit => bool,
  isSymbolicLink: unit => bool,
  isFIFO: unit => bool,
  isSocket: unit => bool,
};
[@bs.module "fs"] external lstatSync: string => Js.t(stat) = "lstatSync";
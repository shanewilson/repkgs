type t =
  | Absolute(string)
  | Relative(string);
let v = s => s->Node.Path.isAbsolute ? Absolute(s) : Relative(s);
let homedir = () => NodeOS.homedir()->v;
let tmpdir = () => NodeOS.tmpdir()->v;
let toString =
  fun
  | Absolute(s)
  | Relative(s) => s;
let pp =
  fun
  | Absolute(s) => s->Js.String2.replace(homedir()->toString, "~")
  | Relative(s) => s;
let pps = ps => ps->List.map(pp)->List.toArray;
let resolve = (~cwd, p) => Node.Path.resolve(cwd->toString, p)->v;
// let cwd = () =>
//   Node.Process.process##env
//   ->Js.Dict.get("__CWD__")
//   ->Js.Option.getWithDefault(Node.Process.cwd(), _)
//   ->Node.Path.resolve("")
//   ->v;
let type_ = p => p === homedir() ? `Homedir : p === tmpdir() ? `Tmpdir : `Path;
let append = (a, b) =>
  switch (b) {
  | Absolute(_) => b
  | Relative(bs) => Node.Path.join2(a->toString, bs)->v
  };
// let isValidSeg = s =>
//   s->Js.String2.includes(Node.Path.sep) ? `InvalidSeg : `ValidSeg;
// let addSeg = (a, b) =>
//   switch (b->isValidSeg) {
//   | `InvalidSeg => raise(InvalidSegment(b))
//   | `ValidSeg => append(a, v(b))
//   };
let relativize = (~cwd, p) =>
  switch (p) {
  | Relative(s) => Relative(s)
  | Absolute(s) => Node.Path.relative(~from=cwd->toString, ~to_=s, ())->v
  };
// let normalize = p => p->Node.Path.normalize->v;
let parent = p => ".."->resolve(~cwd=p);
let segs = p => p->toString->Js.String2.split(Node.Path.sep)->List.fromArray;
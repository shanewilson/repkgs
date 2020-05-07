type t = {
  patterns: List.t(string),
  cwd: Path.t,
};
let pp = t => {"patterns": t.patterns->List.toArray, "cwd": t.cwd->Path.pp};
let v = (patterns, ~cwd) => {patterns, cwd};
let mmatches = (ss, ~patterns) =>
  Micromatch.mm(
    ss->List.toArray,
    patterns->List.toArray,
    Micromatch.t(~contains=true, ()),
  )
  ->List.fromArray;

let mmatch = (s, ~patterns) => [s]->mmatches(~patterns);

let vmatch = glob => {
  glob.patterns
  ->List.toArray
  ->FastGlob.sync(
      FastGlob.t(~cwd=glob.cwd->Path.toString, ~absolute=true, ()),
    )
  ->List.fromArray
  ->List.map(Path.v);
};

let vmatches = globs => {
  globs->List.map(vmatch)->List.flatten;
};

let vmatchesManifest = (globs, ~manifest) => {
  globs
  ->List.map(glob =>
      {
        patterns:
          glob.patterns
          ->List.map(p =>
              p->Path.v->Path.append(manifest->Path.v)->Path.toString
            ),
        cwd: glob.cwd,
      }
    )
  ->vmatches
  ->List.map(Path.parent);
};
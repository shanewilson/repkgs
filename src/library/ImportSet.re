module Import = {
  type import = string;
  type t =
    | Local(import, Path.t)
    | Unresolved(import, Path.t)
    | External(import);
  let v = (import, ~path) => {
    let x = import->String.get(0);
    switch (x) {
    | '.'
    | '/' =>
      let filePath =
        x === '.' ? path->Path.parent->Path.addSeg(import) : import->Path.v;
      filePath
      ->Resolve.resolve
      ->(
          fun
          | Ok(p) => Local(import, p)
          | _ => Unresolved(import, filePath)
        );
    // External could also be ../../node_modules/@thing/beep
    // so need to handle that at some point
    | _ => External(import)
    };
  };
  let toString =
    fun
    | Local(s, _)
    | Unresolved(s, _)
    | External(s) => s;
};

module Comparator =
  Id.MakeComparable({
    type t = Import.t;
    let cmp = (a, b) =>
      switch (a->Import.toString, b->Import.toString) {
      | (x, y) => Pervasives.compare(x, y)
      };
  });

type t = Set.t(Comparator.t, Comparator.identity);
let empty = Set.make(~id=(module Comparator));
let fromArray = Set.fromArray(~id=(module Comparator));
let fromList = x => x->List.toArray->fromArray;
let toArray = Set.toArray;
let toList = Set.toList;
let diff = Set.diff;

let keepLocalImports =
  Set.keep(
    _,
    fun
    | Import.Local(_) => true
    | _ => false,
  );
let keepExternalImports =
  Set.keep(
    _,
    fun
    | Import.External(_) => true
    | _ => false,
  );
let keepBrokenImports =
  Set.keep(
    _,
    fun
    | Import.Unresolved(_) => true
    | _ => false,
  );
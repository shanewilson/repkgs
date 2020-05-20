module Import = {
  type import = {
    path: Path.t,
    target: Path.t,
    import: string,
  };
  type t =
    | Local(import)
    | Unresolved(import)
    | External(import);
  let v = (import, ~path) => {
    let x = import->String.get(0);
    switch (x) {
    | '.'
    | '/' =>
      let target =
        x === '.' ? path->Path.parent->Path.addSeg(import) : import->Path.v;
      target
      ->Resolve.resolve
      ->(
          fun
          | Ok(resolved) => Local({path, target: resolved, import})
          | Error(_) => Unresolved({path, target, import})
        );
    // External could also be ../../node_modules/@thing/beep
    // so need to handle that at some point
    | _ => External({path, target: import->Path.v, import})
    };
  };
  let path =
    fun
    | Local(x)
    | Unresolved(x)
    | External(x) => x.path;
  let import =
    fun
    | Local(x)
    | Unresolved(x)
    | External(x) => x.import;
  let target =
    fun
    | Local(x)
    | Unresolved(x)
    | External(x) => x.target;
};

module Comparator =
  Id.MakeComparable({
    type t = Import.t;
    let cmp = (a, b) =>
      switch (
        a->Import.target->Path.toString,
        b->Import.target->Path.toString,
      ) {
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
let size = Set.size;

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
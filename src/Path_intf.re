module type M = {
  type t;
  let empty: t;
  let v: string => t;
  let pp: t => string;

  let append: (t, t) => t;
  let addSeg: (t, t) => t;
  let normalize: t => t;
  let basename: t => t;
  let filename: t => t;
  let dirname: t => t;
  let parent: t => t;
  let segs: t => list(t);
};
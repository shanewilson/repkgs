module type M = {
  type t;
  let empty: t;
  let v: string => t;
  let pp: t => string;
};
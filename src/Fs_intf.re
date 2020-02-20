module type M = {
  type t('a);

  let exists: Path.t => Belt.Result.t(t('a), 'b);
  let read: Path.t => Belt.Result.t(string, 'b);
  let dirname: Path.t => Path.t;
};
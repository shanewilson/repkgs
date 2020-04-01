module type M = {
  type t('a);
  let empty: t('a);
  let v: (~root: string, ~packageJson: PackageJson.t) => t('a);
  let pp: t('a) => string;

  let dependsOn: t('a) => PackageSet.t;
  let usedBy: list(t('a)) => PackageSet.t;
};
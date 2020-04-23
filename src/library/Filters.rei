let make:
  (
    Belt.List.t(Package.t),
    ~cwd: Path.t,
    ~includePrivate: bool,
    ~nameOnly: Belt.List.t(string),
    ~nameIgnored: Belt.List.t(string),
    ~pathOnly: Belt.List.t(string),
    ~pathIgnored: Belt.List.t(string),
    ~fsOnly: Belt.List.t(string),
    ~fsIgnored: Belt.List.t(string),
    ~since: string,
    ~sinceBranch: string,
    ~sinceLatestTag: bool
  ) =>
  Belt.List.t(Package.t);
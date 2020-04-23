let only =
  fun
  | [] => false
  | _ => true;
let ignored = xs => !xs->only;
let triggerOn = (p, ~xs, ~fn) =>
  switch (xs) {
  | [] => true
  | _ => p->fn
  };
module Include = {
  let includePrivate = (ps, ~includePrivate) =>
    ps->List.keep(
      Package.(
        fun
        | Publishable(_) => true
        | Private(_) => includePrivate
      ),
    );
};
module Name = {
  let only = (ps, ~nameOnly) =>
    ps->List.keep(
      triggerOn(~xs=nameOnly, ~fn=p =>
        p->Package.name->Glob.mmatch(~patterns=nameOnly)->only
      ),
    );
  let ignored = (ps, ~nameIgnored) =>
    ps->List.keep(
      triggerOn(~xs=nameIgnored, ~fn=p =>
        p->Package.name->Glob.mmatch(~patterns=nameIgnored)->ignored
      ),
    );
};
module Dir = {
  let only = (ps, ~pathOnly) =>
    ps->List.keep(
      triggerOn(~xs=pathOnly, ~fn=p =>
        p->Package.path->Path.toString->Glob.mmatch(~patterns=pathOnly)->only
      ),
    );
  let ignored = (ps, ~pathIgnored) =>
    ps->List.keep(
      triggerOn(~xs=pathIgnored, ~fn=p =>
        p
        ->Package.path
        ->Path.toString
        ->Glob.mmatch(~patterns=pathIgnored)
        ->ignored
      ),
    );
};
module Fs = {
  let only = (ps, ~fsOnly) =>
    ps->List.keep(
      triggerOn(~xs=fsOnly, ~fn=p =>
        fsOnly->Glob.v(~cwd=p->Package.path)->Glob.vmatch->only
      ),
    );
  let ignored = (ps, ~fsIgnored) =>
    ps->List.keep(
      triggerOn(~xs=fsIgnored, ~fn=p =>
        fsIgnored->Glob.v(~cwd=p->Package.path)->Glob.vmatch->ignored
      ),
    );
};
module Git = {
  let since = (ps, ~cwd, ~since, ~sinceBranch, ~sinceLatestTag) => {
    let git = Git.findTrackedFiles(~cwd);
    let lines =
      (
        switch (sinceLatestTag, sinceBranch, since) {
        | (false, "", "") => []
        | (false, "", s) => s->git
        | (false, b, _) => (b ++ "...")->git
        | (true, _, _) => (Git.findClosestTag(~cwd) ++ "...")->git
        }
      )
      ->List.map(line => cwd->Path.addSeg(line)->Path.toString);

    ps->List.keep(
      triggerOn(
        ~xs=lines,
        ~fn=pkg => {
          let p = pkg->Package.path->Path.addSeg("**")->Path.toString;
          lines->Glob.mmatches(~patterns=[p])->only;
        },
      ),
    );
  };
};
let make =
    (
      ps,
      ~cwd,
      ~includePrivate,
      ~nameOnly,
      ~nameIgnored,
      ~pathOnly,
      ~pathIgnored,
      ~fsOnly,
      ~fsIgnored,
      ~since,
      ~sinceBranch,
      ~sinceLatestTag,
    ) => {
  ps
  ->Include.includePrivate(~includePrivate)
  ->Name.only(~nameOnly)
  ->Name.ignored(~nameIgnored)
  ->Dir.only(~pathOnly)
  ->Dir.ignored(~pathIgnored)
  ->Git.since(~cwd, ~since, ~sinceBranch, ~sinceLatestTag)
  ->Fs.only(~fsOnly)
  ->Fs.ignored(~fsIgnored);
};
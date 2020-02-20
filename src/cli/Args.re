open Cmdliner;
open Arg;
let errNo = (kind, s) =>
  Printf.sprintf("no %s %s", Cmdliner_base.quote(s), kind);
let errNotDir = s =>
  Printf.sprintf("%s is not a directory", Cmdliner_base.quote(s));
let dir = {
  let parse = s =>
    s->Node.Fs.existsSync
      ? NodeFs.lstatSync(s)##isDirectory() ? `Ok(s) : `Error(errNotDir(s))
      : `Error(errNo("directory", s));
  (parse, Format.pp_print_string);
};

module Common = {
  let docs = Manpage.s_common_options;
  let cwd = {
    let doc = "Run the program in directory $(docv).";
    value
    & opt(dir, Node.Process.cwd())
    & info(["cwd"], ~docs, ~docv="DIR", ~doc, ~env="CWD"->env_var);
  };
  let workspaces = {
    let doc = "Set the directory patterns for $(docv). Supports globs.";
    value
    & opt_all(list(string), [])
    & info(
        ["w", "workspaces"],
        ~docs,
        ~docv="WORKSPACES",
        ~doc,
        ~env="WORKSPACES"->env_var,
      );
  };
};

module Since = {
  let docs = "CHANGED SINCE FILTERS";
  let since = {
    let doc = "Find packages modified since $(docv)";
    value
    & opt(~vopt="HEAD..", string, "")
    & info(["since"], ~docv="COMMIT", ~doc, ~docs);
  };
  let sinceBranch = {
    let doc = "Find packages modified since $(docv). Takes over $(b,--since).";
    value
    & opt(~vopt="master", string, "")
    & info(["since-branch"], ~docv="BRANCH/TAG", ~doc, ~docs);
  };
  let sinceLatestTag = {
    let doc = "Find packages modified since the closest annotated tag. Takes over $(b,--since) and $(b,--since-branch).";
    value & flag & info(["since-latest-tag"], ~doc, ~docs);
  };
};

module Name = {
  let docs = "PACKAGE NAME FILTERS";
  let only = {
    let doc = "Include all packages whose name matches pattern $(docv)";
    value & opt_all(string, []) & info(["only"], ~docv="GLOB", ~doc, ~docs);
  };
  let ignored = {
    let doc = "Exclude any package whose name matches pattern $(docv)";
    value
    & opt_all(string, [])
    & info(["ignore"], ~docv="GLOB", ~doc, ~docs);
  };
};

module Fs = {
  let docs = "FILE SYSTEM FILTERS";
  let only = {
    let doc = "Include all packages with any files matching pattern $(docv)";
    value
    & opt_all(string, [])
    & info(["only-fs"], ~docv="GLOB", ~doc, ~docs);
  };
  let ignored = {
    let doc = "Exclude any package with any files matching pattern $(docv)";
    value
    & opt_all(string, [])
    & info(["ignore-fs"], ~docv="GLOB", ~doc, ~docs);
  };
};

module Path = {
  let docs = "PATH FILTERS";
  let only = {
    let doc = "Include all packages with the path matching pattern $(docv)";
    value
    & opt_all(string, [])
    & info(["only-path"], ~docv="GLOB", ~doc, ~docs);
  };
  let ignored = {
    let doc = "Exclude any package with the path matching pattern $(docv)";
    value
    & opt_all(string, [])
    & info(["ignore-path"], ~docv="GLOB", ~doc, ~docs);
  };
};

module Include = {
  let includePrivate = {
    let doc = "Include private packages.";
    value & flag & info(["include-private"], ~doc);
  };
};
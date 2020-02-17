open TestFramework;

let dirs = [
  "workspace/package.json",
  "workspace/package-a/package.json",
  "workspace/packages/package-b/package.json",
  "workspace/long/path/to/workspaces/package-b/package.json",
  "workspace/long/path/to/workspaces/path/to/package-b/package.json",
];

describe("Glob", ({test, _}) => {
  test("*", ({expect}) => {
    expect.list(
      dirs
      |> List.filter(s =>
           Re.execp(
             Re.seq(
               Library.Glob.create_globs_seq(
                 Fpath.v("workspace/*/package.json"),
               ),
             )
             |> Re.compile,
             s,
           )
         ),
    ).
      toEqual([
      "workspace/package-a/package.json",
    ])
  });
  test("**", ({expect}) => {
    expect.list(
      dirs
      |> List.filter(s =>
           Re.execp(
             Re.seq(
               Library.Glob.create_globs_seq(
                 Fpath.v("workspace/**/package.json"),
               ),
             )
             |> Re.compile,
             s,
           )
         ),
    ).
      toEqual([
      "workspace/package-a/package.json",
      "workspace/packages/package-b/package.json",
      "workspace/long/path/to/workspaces/package-b/package.json",
      "workspace/long/path/to/workspaces/path/to/package-b/package.json",
    ])
  });
  test("**/*", ({expect}) => {
    expect.list(
      dirs
      |> List.filter(s =>
           Re.execp(
             Re.seq(
               Library.Glob.create_globs_seq(
                 Fpath.v("workspace/**/workspaces/*/package.json"),
               ),
             )
             |> Re.compile,
             s,
           )
         ),
    ).
      toEqual([
      "workspace/long/path/to/workspaces/package-b/package.json",
    ])
  });
  test("**/**", ({expect}) => {
    expect.list(
      dirs
      |> List.filter(s =>
           Re.execp(
             Re.seq(
               Library.Glob.create_globs_seq(
                 Fpath.v("workspace/**/workspaces/**/package.json"),
               ),
             )
             |> Re.compile,
             s,
           )
         ),
    ).
      toEqual([
      "workspace/long/path/to/workspaces/package-b/package.json",
      "workspace/long/path/to/workspaces/path/to/package-b/package.json",
    ])
  });
});
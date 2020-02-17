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
      |> Library.Glob.matches(
           ~patterns=[Fpath.v("workspace/*/package.json")],
         ),
    ).
      toEqual([
      "workspace/package-a/package.json",
    ])
  });
  test("**", ({expect}) => {
    expect.list(
      dirs
      |> Library.Glob.matches(
           ~patterns=[Fpath.v("workspace/**/package.json")],
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
      |> Library.Glob.matches(
           ~patterns=[Fpath.v("workspace/**/workspaces/*/package.json")],
         ),
    ).
      toEqual([
      "workspace/long/path/to/workspaces/package-b/package.json",
    ])
  });
  test("**/**", ({expect}) => {
    expect.list(
      dirs
      |> Library.Glob.matches(
           ~patterns=[Fpath.v("workspace/**/workspaces/**/package.json")],
         ),
    ).
      toEqual([
      "workspace/long/path/to/workspaces/package-b/package.json",
      "workspace/long/path/to/workspaces/path/to/package-b/package.json",
    ])
  });

  test("multiple patterns", ({expect}) => {
    expect.list(
      dirs
      |> Library.Glob.matches(
           ~patterns=[
             Fpath.v("workspace/**/workspaces/**/package.json"),
             Fpath.v("workspace/*/package.json"),
           ],
         ),
    ).
      toEqual([
      "workspace/package-a/package.json",
      "workspace/long/path/to/workspaces/package-b/package.json",
      "workspace/long/path/to/workspaces/path/to/package-b/package.json",
    ])
  });
});
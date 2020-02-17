open TestFramework;
// This is mostly to document the behaviour of @opam/re

let dirs = [
  "workspace/package.json",
  "workspace/package-a/package.json",
  "workspace/packages/package-b/package.json",
  "workspace/long/path/to/workspaces/package-b/package.json",
  "workspace/long/path/to/workspaces/path/to/package-b/package.json",
];

describe("Re.Glob", ({test, _}) => {
  test("*:pathname:true", ({expect}) => {
    expect.list(
      dirs
      |> List.filter(s =>
           Re.execp(
             Re.seq([
               Re.Glob.glob("workspace/*/package.json", ~pathname=true),
             ])
             |> Re.compile,
             s,
           )
         ),
    ).
      toEqual([
      "workspace/package-a/package.json",
    ])
  });
  test("*:pathname:false", ({expect}) => {
    expect.list(
      dirs
      |> List.filter(s =>
           Re.execp(
             Re.seq([
               Re.Glob.glob("workspace/*/package.json", ~pathname=false),
             ])
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
  test("**:pathname:true", ({expect}) => {
    expect.list(
      dirs
      |> List.filter(s =>
           Re.execp(
             Re.seq([
               Re.Glob.glob("workspace/**/package.json", ~pathname=true),
             ])
             |> Re.compile,
             s,
           )
         ),
    ).
      toEqual([
      "workspace/package-a/package.json",
    ])
  });
  test("**:pathname:false", ({expect}) => {
    expect.list(
      dirs
      |> List.filter(s =>
           Re.execp(
             Re.seq([
               Re.Glob.glob("workspace/**/package.json", ~pathname=false),
             ])
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
  test("*/*:pathname:true", ({expect}) => {
    expect.list(
      dirs
      |> List.filter(s =>
           Re.execp(
             Re.seq([
               Re.Glob.glob(
                 "workspace/*/workspaces/*/package.json",
                 ~pathname=true,
               ),
             ])
             |> Re.compile,
             s,
           )
         ),
    ).
      toEqual(
      [],
    )
  });
  test("*/*:pathname:false", ({expect}) => {
    expect.list(
      dirs
      |> List.filter(s =>
           Re.execp(
             Re.seq([
               Re.Glob.glob(
                 "workspace/*/workspaces/*/package.json",
                 ~pathname=false,
               ),
             ])
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

  test("**/**:pathname:true", ({expect}) => {
    expect.list(
      dirs
      |> List.filter(s =>
           Re.execp(
             Re.seq([
               Re.Glob.glob(
                 "workspace/**/workspaces/**/package.json",
                 ~pathname=true,
               ),
             ])
             |> Re.compile,
             s,
           )
         ),
    ).
      toEqual(
      [],
    )
  });

  test("**/**:pathname:false", ({expect}) => {
    expect.list(
      dirs
      |> List.filter(s =>
           Re.execp(
             Re.seq([
               Re.Glob.glob(
                 "workspace/**/workspaces/**/package.json",
                 ~pathname=false,
               ),
             ])
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

  test("**/*:pathname:true", ({expect}) => {
    expect.list(
      dirs
      |> List.filter(s =>
           Re.execp(
             Re.seq([
               Re.Glob.glob(
                 "workspace/**/workspaces/*/package.json",
                 ~pathname=true,
               ),
             ])
             |> Re.compile,
             s,
           )
         ),
    ).
      toEqual(
      [],
    )
  });

  test("**/*:pathname:false", ({expect}) => {
    expect.list(
      dirs
      |> List.filter(s =>
           Re.execp(
             Re.seq([
               Re.Glob.glob(
                 "workspace/**/workspaces/*/package.json",
                 ~pathname=false,
               ),
             ])
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

  test("**/*:seq", ({expect}) => {
    expect.list(
      dirs
      |> List.filter(s =>
           Re.execp(
             Re.seq([
               Re.Glob.glob("workspace/**", ~pathname=false),
               Re.Glob.glob("workspaces/*/package.json", ~pathname=true),
             ])
             |> Re.compile,
             s,
           )
         ),
    ).
      toEqual([
      "workspace/long/path/to/workspaces/package-b/package.json",
    ])
  });
});
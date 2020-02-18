open TestFramework;

describe("Glob", ({test, _}) => {
  describe("String", ({test, _}) => {
    let names = [
      "ignore-me",
      "@scope-a/name-a",
      "@scope-a/name-b",
      "@scope-b/name-a",
      "@scope-b/name-b",
      "graphql",
      "graphql-test",
      "graphql-utils",
      "npm-test",
    ];

    test("exact", ({expect}) => {
      expect.list(
        names |> Library.Glob.String.matches(~patterns=["graphql"]),
      ).
        toEqual([
        "graphql",
      ])
    });
    test("prefix*", ({expect}) => {
      expect.list(
        names |> Library.Glob.String.matches(~patterns=["graphql*"]),
      ).
        toEqual([
        "graphql",
        "graphql-test",
        "graphql-utils",
      ])
    });
    test("*suffix", ({expect}) => {
      expect.list(names |> Library.Glob.String.matches(~patterns=["*test"])).
        toEqual([
        "graphql-test",
        "npm-test",
      ])
    });
    test("@scope/*", ({expect}) => {
      expect.list(
        names |> Library.Glob.String.matches(~patterns=["@scope-a/*"]),
      ).
        toEqual([
        "@scope-a/name-a",
        "@scope-a/name-b",
      ])
    });
    test("@scope**", ({expect}) => {
      expect.list(
        names |> Library.Glob.String.matches(~patterns=["@scope-a**"]),
      ).
        toEqual([
        "@scope-a/name-a",
        "@scope-a/name-b",
      ])
    });
    test("*/scoped", ({expect}) => {
      expect.list(
        names |> Library.Glob.String.matches(~patterns=["*/name-a"]),
      ).
        toEqual([
        "@scope-a/name-a",
        "@scope-b/name-a",
      ])
    });
    test("**scoped", ({expect}) => {
      expect.list(
        names |> Library.Glob.String.matches(~patterns=["**name-a"]),
      ).
        toEqual([
        "@scope-a/name-a",
        "@scope-b/name-a",
      ])
    });
    test("{expand}", ({expect}) => {
      expect.list(
        names
        |> Library.Glob.String.matches(~patterns=["@scope-a/name-{a,b}"]),
      ).
        toEqual([
        "@scope-a/name-a",
        "@scope-a/name-b",
      ])
    });
  });
  describe("Path", ({test, _}) => {
    let dirs =
      [
        "ignore/me",
        "workspace/package.json",
        "workspace/package-a/package.json",
        "workspace/packages/package-a/package.json",
        "workspace/packages/package-b/package.json",
        "workspace/long/path/to/workspaces/package-b/package.json",
        "workspace/long/path/to/workspaces/path/to/package-b/package.json",
      ]
      |> List.map(Fpath.v);
    test("*", ({expect}) => {
      expect.list(
        dirs
        |> Library.Glob.Path.matches(~patterns=["workspace/*/package.json"]),
      ).
        toEqual(
        ["workspace/package-a/package.json"] |> List.map(Fpath.v),
      )
    });
    test("**", ({expect}) => {
      expect.list(
        dirs
        |> Library.Glob.Path.matches(~patterns=["workspace/**/package.json"]),
      ).
        toEqual(
        [
          // this isn't to spec but waiting on real globstar support
          "workspace/package.json",
          "workspace/package-a/package.json",
          "workspace/packages/package-a/package.json",
          "workspace/packages/package-b/package.json",
          "workspace/long/path/to/workspaces/package-b/package.json",
          "workspace/long/path/to/workspaces/path/to/package-b/package.json",
        ]
        |> List.map(Fpath.v),
      )
    });
    test("**/*", ({expect}) => {
      expect.list(
        dirs
        |> Library.Glob.Path.matches(
             ~patterns=["workspace/**/workspaces/*/package.json"],
           ),
      ).
        toEqual(
        ["workspace/long/path/to/workspaces/package-b/package.json"]
        |> List.map(Fpath.v),
      )
    });
    test("*/**", ({expect}) => {
      expect.list(
        dirs
        |> Library.Glob.Path.matches(
             ~patterns=["workspace/*/path/**/package.json"],
           ),
      ).
        toEqual(
        [
          "workspace/long/path/to/workspaces/package-b/package.json",
          "workspace/long/path/to/workspaces/path/to/package-b/package.json",
        ]
        |> List.map(Fpath.v),
      )
    });
    test("**/**", ({expect}) => {
      expect.list(
        dirs
        |> Library.Glob.Path.matches(
             ~patterns=["workspace/**/workspaces/**/package.json"],
           ),
      ).
        toEqual(
        [
          "workspace/long/path/to/workspaces/package-b/package.json",
          "workspace/long/path/to/workspaces/path/to/package-b/package.json",
        ]
        |> List.map(Fpath.v),
      )
    });

    test("multiple patterns", ({expect}) => {
      expect.list(
        dirs
        |> Library.Glob.Path.matches(
             ~patterns=[
               "workspace/**/workspaces/**/package.json",
               "workspace/*/package.json",
             ],
           ),
      ).
        toEqual(
        [
          "workspace/package-a/package.json",
          "workspace/long/path/to/workspaces/package-b/package.json",
          "workspace/long/path/to/workspaces/path/to/package-b/package.json",
        ]
        |> List.map(Fpath.v),
      )
    });

    test("{expand}", ({expect}) => {
      expect.list(
        dirs
        |> Library.Glob.Path.matches(
             ~patterns=["workspace/packages/package-{a,b}/package.json"],
           ),
      ).
        toEqual(
        [
          "workspace/packages/package-a/package.json",
          "workspace/packages/package-b/package.json",
        ]
        |> List.map(Fpath.v),
      )
    });
  });
});
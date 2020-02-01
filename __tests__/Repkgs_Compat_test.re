open Jest;
open Expect;

open Repkgs.Compat;

let fixturesDir = "__fixtures__";

let patterns = [|
  "workspace-a/packages/*",
  "workspace-b/package-a",
  "workspace-b/package-b",
|];

let dirs = [|
  "workspace-a/packages/package-a",
  "workspace-a/packages/package-b",
  "workspace-b/package-a",
  "workspace-b/package-b",
|];

let compareArrays = (arr1, arr2) =>
  arr1
  |> Belt.SortArray.String.stableSort
  |> expect
  |> toEqual(arr2 |> Belt.SortArray.String.stableSort);

let comparePaths = (cwd, fn: string => array(Common.matched), arr2) =>
  cwd
  ->fn
  ->Belt.Array.map(ws => Node.Path.relative(~from=cwd, ~to_=ws.absolute, ()))
  ->compareArrays(arr2);

describe("Compat", () => {
  describe("Pnpm", () => {
    let dir = "pnpm";
    let cwd = Node.Path.join2(fixturesDir, "pnpm");

    test("patterns", () =>
      compareArrays(patterns, cwd->Pnpm.patterns)
    );

    describe("findRoot", () => {
      test("find root at root", () =>
        cwd
        |> Pnpm.findRoot
        |> Belt.Result.getExn
        |> Node.Path.relative(~from=fixturesDir, ~to_=_, ())
        |> expect
        |> toEqual(dir)
      );
      test("find root from subdirectory", () =>
        Node.Path.join([|cwd, "workspace-a", "package-a"|])
        |> Pnpm.findRoot
        |> Belt.Result.getExn
        |> Node.Path.relative(~from=fixturesDir, ~to_=_, ())
        |> expect
        |> toEqual(dir)
      );
      test("throw when no workspace found", () =>
        "/nothing"
        |> Pnpm.findRoot
        |> Belt.Result.isError
        |> expect
        |> toEqual(true)
      );
    });

    test("packages", () =>
      comparePaths(cwd, Pnpm.packages, dirs)
    );
  });
  describe("Rush", () => {
    let dir = "rush";
    let cwd = Node.Path.join2(fixturesDir, dir);
    test("patterns", () =>
      compareArrays(cwd->Rush.patterns, dirs)
    );

    describe("findRoot", () => {
      test("find root at root", () =>
        cwd
        |> Rush.findRoot
        |> Belt.Result.getExn
        |> Node.Path.relative(~from=fixturesDir, ~to_=_, ())
        |> expect
        |> toEqual(dir)
      );
      test("find root from subdirectory", () =>
        Node.Path.join([|cwd, "workspace-a", "package-a"|])
        |> Rush.findRoot
        |> Belt.Result.getExn
        |> Node.Path.relative(~from=fixturesDir, ~to_=_, ())
        |> expect
        |> toEqual(dir)
      );
      test("throw when no workspace found", () =>
        "/nothing"
        |> Rush.findRoot
        |> Belt.Result.isError
        |> expect
        |> toEqual(true)
      );
    });

    test("packages", () =>
      comparePaths(cwd, Rush.packages, dirs)
    );
  });
  describe("Yarn_V1", () => {
    let dir = "yarn";
    let cwd = Node.Path.join2(fixturesDir, dir);
    test("patterns", () =>
      compareArrays(cwd->Yarn_V1.patterns, patterns)
    );

    describe("findRoot", () => {
      test("find root at root", () =>
        cwd
        |> Yarn_V1.findRoot
        |> Belt.Result.getExn
        |> Node.Path.relative(~from=fixturesDir, ~to_=_, ())
        |> expect
        |> toEqual(dir)
      );
      test("find root from subdirectory", () =>
        Node.Path.join([|cwd, "workspace-a", "package-a"|])
        |> Yarn_V1.findRoot
        |> Belt.Result.getExn
        |> Node.Path.relative(~from=fixturesDir, ~to_=_, ())
        |> expect
        |> toEqual(dir)
      );
      test("throw when no workspace found", () =>
        "/nothing"
        |> Yarn_V1.findRoot
        |> Belt.Result.isError
        |> expect
        |> toEqual(true)
      );
    });

    test("packages", () =>
      comparePaths(cwd, Yarn_V1.packages, dirs)
    );
  });
  describe("Yarn_V2", () => {
    let dir = "berry";
    let cwd = Node.Path.join2(fixturesDir, dir);

    describe("findRoot", () => {
      test("find root at root", () =>
        cwd
        |> Yarn_V2.findRoot
        |> Belt.Result.getExn
        |> Node.Path.relative(~from=fixturesDir, ~to_=_, ())
        |> expect
        |> toEqual(dir)
      );
      test("find root from subdirectory", () =>
        Node.Path.join([|cwd, "workspace-a", "package-a"|])
        |> Yarn_V2.findRoot
        |> Belt.Result.getExn
        |> Node.Path.relative(~from=fixturesDir, ~to_=_, ())
        |> expect
        |> toEqual(dir)
      );
      test("throw when no workspace found", () =>
        "/nothing"
        |> Yarn_V2.findRoot
        |> Belt.Result.isError
        |> expect
        |> toEqual(true)
      );
    });

    test("packages", () =>
      comparePaths(
        cwd,
        Yarn_V2.packages,
        [|
          "",
          "workspace-a",
          "workspace-a/packages/package-a",
          "workspace-a/packages/package-b",
          "workspace-b",
          "workspace-b/package-a",
          "workspace-b/package-b",
        |],
      )
    );
  });
  describe("detect", () => {
    describe("Pnpm workspace", () => {
      let dir = "pnpm";
      let cwd = Node.Path.join2(fixturesDir, dir);
      test("Pnpm", () =>
        cwd |> Pnpm.detect |> expect |> toBe(true)
      );
      test("Rush", () =>
        cwd |> Rush.detect |> expect |> toBe(false)
      );
      test("Yarn_V1", () =>
        cwd |> Yarn_V1.detect |> expect |> toBe(false)
      );
      test("Yarn_V2", () =>
        cwd |> Yarn_V2.detect |> expect |> toBe(false)
      );
    });
    describe("Rush workspace", () => {
      let dir = "rush";
      let cwd = Node.Path.join2(fixturesDir, dir);
      test("Pnpm", () =>
        cwd |> Pnpm.detect |> expect |> toBe(false)
      );
      test("Rush", () =>
        cwd |> Rush.detect |> expect |> toBe(true)
      );
      test("Yarn_V1", () =>
        cwd |> Yarn_V1.detect |> expect |> toBe(false)
      );
      test("Yarn_V2", () =>
        cwd |> Yarn_V2.detect |> expect |> toBe(false)
      );
    });
    describe("Yarn_V1 workspace", () => {
      let dir = "yarn";
      let cwd = Node.Path.join2(fixturesDir, dir);
      test("Pnpm", () =>
        cwd |> Pnpm.detect |> expect |> toBe(false)
      );
      test("Rush", () =>
        cwd |> Rush.detect |> expect |> toBe(false)
      );
      test("Yarn_V1", () =>
        cwd |> Yarn_V1.detect |> expect |> toBe(true)
      );
      test("Yarn_V2", () =>
        cwd |> Yarn_V2.detect |> expect |> toBe(false)
      );
    });
    describe("Yarn_V2 workspace", () => {
      let dir = "berry";
      let cwd = Node.Path.join2(fixturesDir, dir);
      test("Pnpm", () =>
        cwd |> Pnpm.detect |> expect |> toBe(false)
      );
      test("Rush", () =>
        cwd |> Rush.detect |> expect |> toBe(false)
      );
      test("Yarn_V1", () =>
        cwd |> Yarn_V1.detect |> expect |> toBe(false)
      );
      test("Yarn_V2", () =>
        cwd |> Yarn_V2.detect |> expect |> toBe(true)
      );
    });
  });
});
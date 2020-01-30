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

let comparePaths = (cwd, fn, arr2) =>
  cwd
  |> fn
  |> Belt.Array.map(_, to_ => Node.Path.relative(~from=cwd, ~to_, ()))
  |> compareArrays(arr2);

describe("Compat", () => {
  describe("Pnpm", () => {
    let cwd = Node.Path.join2(fixturesDir, "pnpm");
    test("patterns", () =>
      compareArrays(Pnpm.patterns([|cwd, Pnpm.manifest|]), patterns)
    );
    test("packages", () =>
      comparePaths(cwd, Pnpm.packages, dirs)
    );
  });
  describe("Rush", () => {
    let cwd = Node.Path.join2(fixturesDir, "rush");
    test("patterns", () =>
      compareArrays(Rush.patterns([|cwd, Rush.manifest|]), dirs)
    );
    test("packages", () =>
      comparePaths(cwd, Rush.packages, dirs)
    );
  });
  describe("Yarn_V1", () => {
    let cwd = Node.Path.join2(fixturesDir, "yarn");
    test("patterns", () =>
      compareArrays(Yarn_V1.patterns([|cwd, Yarn_V1.manifest|]), patterns)
    );
    test("packages", () =>
      comparePaths(cwd, Yarn_V1.packages, dirs)
    );
  });
  describe("Yarn_V2", () => {
    let cwd = Node.Path.join2(fixturesDir, "berry");
    test("packages", () =>
      comparePaths(cwd, Yarn_V2.packages, dirs)
    );
  });
});
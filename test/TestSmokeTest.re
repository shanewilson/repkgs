open TestFramework;

describe("Integration Test `--version`", ({test, _}) => {
  test("Get version", ({expect}) => {
    let version = TestUtils.run([|"--version"|]);
    let generated = expect.string(version);
    generated.toMatch("^\\d+.\\d+.\\d+");
  })
});

describe("Integration Test `list`", ({test, _}) => {
  test("Yarn v2", ({expect}) => {
    let ws = TestUtils.run([|"list", "--cwd", "__fixtures__/yarn_v2"|]);
    expect.string(ws).toMatchSnapshot();
  });
  test("Yarn v1", ({expect}) => {
    let ws = TestUtils.run([|"list", "--cwd", "__fixtures__/yarn_v1"|]);
    expect.string(ws).toMatchSnapshot();
  });
  test("Pnpm", ({expect}) => {
    let ws = TestUtils.run([|"list", "--cwd", "__fixtures__/pnpm"|]);
    expect.string(ws).toMatchSnapshot();
  });
  test("Rush", ({expect}) => {
    let ws = TestUtils.run([|"list", "--cwd", "__fixtures__/rush"|]);
    expect.string(ws).toMatchSnapshot();
  });

  describe("Nested folder", ({test, _}) => {
    test("Yarn v2", ({expect}) => {
      let ws =
        TestUtils.run([|
          "list",
          "--cwd",
          "__fixtures__/yarn_v2/workspace-a/packages/package-a",
        |]);
      expect.string(ws).toMatchSnapshot();
    });
    test("Yarn v1", ({expect}) => {
      let ws =
        TestUtils.run([|
          "list",
          "--cwd",
          "__fixtures__/yarn_v1/workspace-a/packages/package-a",
        |]);
      expect.string(ws).toMatchSnapshot();
    });
    test("Pnpm", ({expect}) => {
      let ws =
        TestUtils.run([|
          "list",
          "--cwd",
          "__fixtures__/pnpm/workspace-a/packages/package-a",
        |]);
      expect.string(ws).toMatchSnapshot();
    });
    test("Rush", ({expect}) => {
      let ws =
        TestUtils.run([|
          "list",
          "--cwd",
          "__fixtures__/rush/workspace-a/packages/package-a",
        |]);
      expect.string(ws).toMatchSnapshot();
    });
  });

  describe("--include-worktree", ({test, _}) => {
    test("Yarn v2", ({expect}) => {
      let ws =
        TestUtils.run([|
          "list",
          "--cwd",
          "__fixtures__/yarn_v2",
          "--include-worktree"
        |]);
      expect.string(ws).toMatchSnapshot();
    });
    test("Yarn v1", ({expect}) => {
      let ws =
        TestUtils.run([|
          "list",
          "--cwd",
          "__fixtures__/yarn_v1",
          "--include-worktree"
        |]);
      expect.string(ws).toMatchSnapshot();
    });
    test("Pnpm", ({expect}) => {
      let ws =
        TestUtils.run([|
          "list",
          "--cwd",
          "__fixtures__/pnpm",
          "--include-worktree"
        |]);
      expect.string(ws).toMatchSnapshot();
    });
    test("Rush", ({expect}) => {
      let ws =
        TestUtils.run([|
          "list",
          "--cwd",
          "__fixtures__/rush",
          "--include-worktree"
        |]);
      expect.string(ws).toMatchSnapshot();
    });
  });
});
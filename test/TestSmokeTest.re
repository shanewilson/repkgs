open TestFramework;

describe("Integration Test `--version`", ({test, _}) => {
  test("Get version", ({expect}) => {
    let version = TestUtils.run([|"--version"|]);
    let generated = expect.string(version);
    generated.toMatch("^\\d+.\\d+.\\d+");
  })
});

describe("Integration Test `Compat`", ({test, _}) => {
  describe("discover workspace manager", ({test, _}) => {
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
  });
  describe("discover root from nested folder", ({test, _}) => {
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
});

describe("Integration Test `list`", ({test, _}) => {
  test("--cwd", ({expect}) => {
    let ws = TestUtils.run([|"list", "--cwd", "__fixtures__/yarn_v2"|]);
    expect.string(ws).toMatchSnapshot();
  });

  test("--include-worktree", ({expect}) => {
    let ws =
      TestUtils.run([|
        "list",
        "--cwd",
        "__fixtures__/yarn_v2",
        "--include-worktree",
      |]);
    expect.string(ws).toMatchSnapshot();
  });

  test("--include", ({expect}) => {
    let ws =
      TestUtils.run([|
        "list",
        "--cwd",
        "__fixtures__/yarn_v2",
        "--include",
        "@workspace-a/*",
      |]);
    expect.string(ws).toMatchSnapshot();
  });

  test("--include --include", ({expect}) => {
    let ws =
      TestUtils.run([|
        "list",
        "--cwd",
        "__fixtures__/yarn_v2",
        "--include",
        "@workspace-a/*",
        "--include",
        "*/package-a",
      |]);
    expect.string(ws).toMatchSnapshot();
  });

  test("--exclude", ({expect}) => {
    let ws =
      TestUtils.run([|
        "list",
        "--cwd",
        "__fixtures__/yarn_v2",
        "--exclude",
        "@workspace-a/*",
      |]);
    expect.string(ws).toMatchSnapshot();
  });

  test("--exclude --exclude", ({expect}) => {
    let ws =
      TestUtils.run([|
        "list",
        "--cwd",
        "__fixtures__/yarn_v2",
        "--exclude",
        "@workspace-a/*",
        "--exclude",
        "*/package-a",
      |]);
    expect.string(ws).toMatchSnapshot();
  });

  test("--include --exclude", ({expect}) => {
    let ws =
      TestUtils.run([|
        "list",
        "--cwd",
        "__fixtures__/yarn_v2",
        "--include",
        "@workspace-a/*",
        "--exclude",
        "*/package-a",
      |]);
    expect.string(ws).toMatchSnapshot();
  });
});
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
    let ws = TestUtils.run([|"list", "--cwd", "__fixtures__/berry"|]);
    expect.string(ws).toMatchSnapshot();
  });
  test("Yarn v1", ({expect}) => {
    let ws = TestUtils.run([|"list", "--cwd", "__fixtures__/yarn"|]);
    expect.string(ws).toMatchSnapshot();
  });

  describe("Nested folder", ({test, _}) => {
    test("Yarn v2", ({expect}) => {
      let ws =
        TestUtils.run([|
          "list",
          "--cwd",
          "__fixtures__/berry/workspace-a/packages/package-a",
        |]);
      expect.string(ws).toMatchSnapshot();
    });
    test("Yarn v1", ({expect}) => {
      let ws =
        TestUtils.run([|
          "list",
          "--cwd",
          "__fixtures__/yarn/workspace-a/packages/package-a",
        |]);
      expect.string(ws).toMatchSnapshot();
    });
  });
});
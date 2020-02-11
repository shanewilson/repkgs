open TestFramework;

describe("Integration Test `spin-cli hello`", ({test, _}) => {
  test("Validate standard output", ({expect}) => {
    let output = TestUtils.run([|"hello", "World"|]);
    let generated = expect.string(output);
    generated.toMatch("Hello World!");
  })
});


describe("Integration Test `--version`", ({test, _}) => {
  test("Get version", ({expect}) => {
    let version = TestUtils.run([|"--version"|]);
    let generated = expect.string(version);
    generated.toMatch("^\\d+.\\d+.\\d+");
  })
});

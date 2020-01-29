<h1 align="center" border="none">
  re|pkgs
  <br/>
  <br/>
  📦
  <br/>
  <br/>
</h1>

<p align="center">
  Create tool agnostic monorepo workflows
</p>

<p align="center">
  <img alt="Build status" src="https://github.com/shanewilson/repkgs/workflows/CI/badge.svg">
</p>

## Getting Started

### Development

```
❯ yarn install
❯ yarn clean && yarn build -w
```

### Tests

> Test *must* be run with `npm`. Running the tests with `yarn` will pin the version of yarn to your global version. This is a problem when trying to run tests against both yarn v1 and v2.

```
❯ npm test --watch
```
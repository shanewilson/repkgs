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

### Bucklescript

#### Development

```
❯ yarn install
❯ yarn clean && yarn build -w
```

#### CLI

```
❯ yarn repkgs list --cwd "__fixtures__/berry"
yarn run v1.21.1
$ node -r esm ./src/RepkgsCli.bs.js list --cwd __fixtures__/berry
verbosity = normal
cwd = __fixtures__/berry
@workspace-a/package-a
@workspace-a/package-b
@workspace-a/worktree
@workspace-b/package-a
@workspace-b/package-b
@workspace-b/worktree
@berry/worktree
✨  Done in 0.65s.
```

#### Tests

```
❯ yarn test --watch
```

### Native

#### Development

```
❯ esy install
❯ esy
```

#### CLI

```
❯ esy start hello Tom -vv
Hello Tom!
RePkgs.exe: [INFO] Hello Tom!
RePkgs.exe: [WARNING] Hello Tom!
RePkgs.exe: [ERROR] Hello Tom!
RePkgs.exe: [DEBUG] Hello Tom!
```

#### Tests

```
❯ esy test
```
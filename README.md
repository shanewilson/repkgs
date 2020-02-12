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

## What is this?

Adds a common interface to running commands in a monorepo that is compatible with Yarn v1 and v2 Workspaces, Pnpm and Rush.

#### Yarn v2.x nested workspaces

```sh
❯ repkgs  --cwd __fixtures__/yarn_v2 -v --include-worktree
RePkgs.exe: [INFO] Detected workspace manager: Yarn
@yarn-v2/worktree (root)
@workspace-a/worktree (worktree)
@workspace-a/package-a
@workspace-a/package-b
@workspace-b/worktree (worktree)
@workspace-b/package-a
@workspace-b/package-b
```

#### Yarn v1.x workspaces

```sh
❯ repkgs list --cwd __fixtures__/yarn_v1 -v
[INFO] Detected workspace manager: Yarn
@workspace-a/package-a
@workspace-a/package-b
@workspace-b/package-a
@workspace-b/package-b
```

#### Pnpm

```sh
❯ repkgs list --cwd __fixtures__/pnpm -v --include-worktree
[INFO] Detected workspace manager: Pnpm
@pnpm/worktree (root)
@workspace-a/package-a
@workspace-a/package-b
@workspace-b/package-a
@workspace-b/package-b
```

#### Rush

```sh
❯ repkgs list --cwd __fixtures__/rush -v
[INFO] Detected workspace manager: Rush
@workspace-a/package-a
@workspace-a/package-b
@workspace-b/package-a
@workspace-b/package-b
```


## Development

```
❯ esy install
❯ esy
```

#### CLI

```
❯ esy start list --cwd __fixtures__/yarn
@yarn/worktree
@workspace-a/worktree
@workspace-a/package-a
@workspace-a/package-b
@workspace-b/worktree
@workspace-b/package-a
@workspace-b/package-b
```

#### Tests

```
❯ esy test
```


### Bucklescript

#### Development

```
❯ yarn install
❯ yarn clean && yarn build -w
```

#### CLI

```
❯ yarn repkgs list --cwd __fixtures__/berry
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

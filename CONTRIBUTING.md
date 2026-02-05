# Contributing to PawSpective Backend

Thank you for your interest in contributing to PawSpective Backend. We don't
know who you are, and what you are doing here, but okay, let's get started!

## There are many rules in our fighting club

- First rule: every task must have a GitHub issue.
- Second rule: Branches must be created from an Issue and include its ID,
example: `feature/52-my-feature`.
- Third rule: each pull request must reference exactly one issue.
- Fourth rule: If you really want to push to main (master), you still can`t.

## Commit Standard

So you start developing, make your first commit, but what should you write in
the commit message?

Here are a few rules:

Format: `<type>(<scope>): <description>`, example: `feat(user): add new feature`.
`<scope>` is optional.

### `<type>`

This describes the kind of change that this commit providing:

- feat: A new feature (adding a new component, providing new variants for
an existing component, etc.).
- fix: A bug fix (correcting a styling issue,
addressing a bug in a component's API, etc.).
When updating non-dev dependencies, mark your changes with the fix: type.
- docs: Documentation-only changes.
- style: Changes that do not affect the meaning of the code
(whitespace, formatting, missing semicolons, etc). Not to be used for UI changes
as those are meaningful changes, consider using feat: of fix: instead.
- refactor: A code change that neither fixes a bug nor adds a feature.
- perf: A code change that improves performance.
- test: Adding missing tests or correcting existing tests.
- build: Changes that affect the build system.
- ci: Changes to our CI/CD configuration files and scripts.
- chore: Other changes that don't modify source or test files. Use this type
when adding or
updating dev dependencies.
- revert: Reverts a previous commit.

Each commit type can have an optional scope to specify the place of the commit
change: type(scope):. It is up to you to add or omit a commit's scope.
When a commit affects a specific component, use the component's
PascalCase name as the commit's scope. For example:

`feat(statusbar): automatically switch pipelines`

### `<scope>`

Scope can be anything specifying the place of the commit change.
For example events, kafka, userModel, authorization, etc

### `<description>`

This is a very short description of the change

- use imperative, present tense: "change" not "changed" nor "changes"
- don't capitalize first letter
- no dot (.) at the end

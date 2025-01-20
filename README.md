# Full-fledged AUI App Template

> [!NOTE]
> Click the <kbd>Use this template</kbd> button to setup your own AUI app project!

It is a template repository that provides a pure template to make it easier to create a new AUI-based application
project.

The main goal of this template is to speed up the setup phase of application development for both new and experienced
developers by preconfiguring the project scaffold and CI set up on GitHub.

# Getting Started

It is worth mentioning the benefits of using GitHub Templates. By created a new project using a template, you start with
no history or reference to this repository.


# Continuous Integration

In this project, continuous integration relies on [GitHub Actions][gh:actions], a set of workflows that enable
automation of building, testing and releasing process.

In the `.github/workflows` directory, you can find definitions for the following GitHub Actions workflows:
- [Build](.github/workflows/build.yml)
  - Triggered on `push` and `pull_request` events.


# Code Quality and Formatting

The template includes `.clang-format` and `.clang-tidy`, the latter is used for code quality checks.

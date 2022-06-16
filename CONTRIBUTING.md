# Contributing Guidelines for `MARTY`


## Make suggestions (users)

To report a bug or make a suggestion about `MARTY` please open a new [issue](https://github.com/docbrown1955/marty-public/issues) if no existing issue addresses the same topic.
## Contribute actively to the project (developers)

If you want to correct or add a new feature yourself, feel free to contribute directly!

First open a new issue [here](https://github.com/docbrown1955/marty-public/issues/new) describing what you want to do to engage discussions.

Then create a new branch locally and start working on it
```
    git checkout -b my-cool-new-feature
```
You can work on this branch, commit, and push on the remote host. The first time you push your work, set the remote branch accordingly:
```
    git push --set-upstream origin my-cool-new-feature
```

You can then open a new [Pull Request](https://github.com/docbrown1955/marty-public/pulls) as a **draft**. In this PR, explain in details the changes you made to the code. Once you consider your work is finished and good enough to be integrated in `MARTY`, mark the PR *'ready for review'*.

A maintainer will review your work and eventually merge your code in `MARTY`, congratulations!

## Release a new version of MARTY (maintainers)

Make sure that all these steps are done before releasing a new version:

- Update the README.md for possible changes in build / dependencies
- Update the website accordingly (release news)
- Update the manual for new / modified features
- Update the version strings in file documentation
- Update the MARTY_VERSION_STR definition in `src/marty/core/marty.h`

The two last items could probably be automated in a CI/CD job.

The release in itself should be done on github, using a [tag](https://github.com/docbrown1955/marty-public/tags)
and defining the associated [release](https://github.com/docbrown1955/marty-public/releases).

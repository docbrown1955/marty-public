# Development container

Configuration for a VS Code dev container with the full MARTY toolchain:

- `Dockerfile`: a Fedora 41 image (GCC 14) with the build dependencies.
- `devcontainer.json`: container name, build settings, run arguments (`--userns=keep-id` for
  Podman), VS Code extensions, and the post-create command.

Open the repository with "Reopen in Container" in VS Code.

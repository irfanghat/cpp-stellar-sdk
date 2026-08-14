## Stellar C++ SDK (`cpp-stellar-sdk`)

`cpp-stellar-sdk` is a C++ library for communicating with [Stellar Horizon](https://developers.stellar.org/docs/data/apis/horizon) and [Stellar RPC](https://developers.stellar.org/docs/data/apis/rpc).

### Developing

The project uses **Dev Containers** to provide a consistent development environment across systems.

#### Prerequisites

* [VS Code](https://code.visualstudio.com/)
* [Docker](https://www.docker.com/)
* [Dev Containers extension for VS Code](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

#### Getting Started

Clone the repository:

```sh
git clone https://github.com/irfanghat/cpp-stellar-sdk.git
cd cpp-stellar-sdk
```

Open the repository in VS Code. You should be prompted to reopen the workspace inside the Dev Container.

Alternatively, open the **Command Palette** and select:

**Dev Containers: Reopen in Container**

Wait for the container to build and initialize. Once it is ready, you can begin developing with the configured C++ toolchain and dependencies.

#### Building the Project

Build the project using CMake and Ninja:

```sh
cmake -S . -B build -G Ninja
cmake --build build
```

### License
Apache-2.0
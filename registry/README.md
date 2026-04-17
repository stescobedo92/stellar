# Stellar vcpkg registry

This folder is a **vcpkg custom registry** for the Stellar library. Consumers
add it to their `vcpkg-configuration.json`, then install via `vcpkg install stellar`.

## Consume from your project

```jsonc
// vcpkg-configuration.json next to your vcpkg.json
{
  "default-registry": {
    "kind": "git",
    "baseline": "<vcpkg-commit-sha>",
    "repository": "https://github.com/microsoft/vcpkg"
  },
  "registries": [
    {
      "kind": "git",
      "repository": "https://github.com/stescobedo92/stellar",
      "baseline": "<commit-sha-of-this-repo-main>",
      "reference": "main",
      "packages": [ "stellar" ]
    }
  ]
}
```

```jsonc
// vcpkg.json
{
  "name": "my-app",
  "version": "0.0.1",
  "dependencies": [ "stellar" ]
}
```

Then in CMake:

```cmake
find_package(stellar CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE stellar::stellar)
```

The port files live under [`ports/stellar/`](ports/stellar) and the version
ledger under [`versions/`](versions). Both are updated automatically by the
`publish-vcpkg` GitHub Action on every release.

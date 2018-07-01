Build Local Docker Image for Testing
====================================
*This document shows how the included `Dockerfile` is used to build a local
docker image with compiled `turbofec` for testing/benchmark.*
  1. Build the Docker Image
  2. Custom Build
  3. Save Docker Image
  4. Load Docker Image
  5. Run Local Docker
  6. Default Build Arguments


Build the docker image
======================
```
  docker build -t neofob/turbofec .
```


Custom Build
============
```
  docker build --build-arg GIT_URL=my_local_git_repo --build-arg GIT_COMMIT=testing-branch -t neofob/turbofec .
```


Save the Built Docker Image to a `tar.xz` file
==============================================
```
  docker save neofob/turbofec:latest | xz -z - > neofob_turbofec.tar.xz
```


Load the Built Docker Image on Another Machine
==============================================
```
  xzcat neofob_turbofec.tar.xz | docker load
```


Run Local Docker for Benchmark
==============================
Benchmarking with 4 jobs
```
  docker run -t --rm neofob/turbofec -j 4
```


Default Docker Build Arguments
==============================
| `ARG` | `Value` |
|:-----:|:-------:|
| `GIT_URL` | `https://github.com/ttsou/turbofec`|
| `GIT_COMMIT` | `master` |


**Note:** The compiled binaries are at `/src/turbofec` directory in the docker image.

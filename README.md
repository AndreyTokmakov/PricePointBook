# PricePointBook
Demo PricePointBook

1. [Overview](#Overview)
2. [Download](#Download)
3. [Requirements](#Requirements)
4. [Build](#Build)
5. [How to run tests](#Tests)
8. [Continuous Integration](#CI)

<a name="Overview"></a>
## Overview
Is C++ implementation of the simple movie booking service: to book seats for the selected movie at the selected theater
Service should be able to:
The of that is to try implement a data structure capable of handling events from the Binance book dept stream
    
   https://binance-docs.github.io/apidocs/spot/en/#diff-depth-stream

<a name="Download"></a>
## Download project:
- Copy/clone the project repo: `git clone https://github.com/AndreyTokmakov/PricePointBook.git`
- move to the project folder: `cd PricePointBook`


<a name="Requirements"></a>
## Project requirements
- C++20
- Cmake
- Boost Libraries (BoostTest framework - used for tests)
- JSON Parser lib https://github.com/nlohmann/json


<a name="Build"></a>
## Build
<a name="linux_build"></a>
### Linux
- Clone the project repo: `git clone https://github.com/AndreyTokmakov/PricePointBook.git`
- Clone JSON Parser repo: `git clone https://github.com/nlohmann/json.git'
- Move to the project folder: `cd PricePointBook`
- Create and move to the build folder: `mkdir build && cd build`
- Configure project build (with cmake): `cmake -DCMAKE_BUILD_TYPE=Release ..`
- Build and compile solution: `make`,
  alternatively, the number of CPUs can be specified to speed up the build `make -j8`


<a name="Tests"></a>
## How to run tests
- Move to the build folder: `cd build/tests`
- Run the unit tests: `./unit_tests -p`
  </br></br>![UnitTestsLocal](https://github.com/AndreyTokmakov/PricePointBook/blob/metadata/images/unit_tests_run.png)
- Run the unit tests: `./benchmarks -p`
  </br></br>![UnitTestsLocal](https://github.com/AndreyTokmakov/PricePointBook/blob/metadata/images/benchmarks.png)
- Run the unit tests: `./memory_usage -p`
  </br></br>![UnitTestsLocal](https://github.com/AndreyTokmakov/PricePointBook/blob/metadata/images/memory_usage_run.png)


<a name="CI"></a>
## Continuous Integration
**GitHub Actions** is used for Quality Assurance purposes and to receive a rapid
feedback on the status of the project and the presence/absence of kind of regression.
1. PricePointBook recent [workflows](https://github.com/AndreyTokmakov/PricePointBook/actions) status <br />
2. Builds and tests are configured to run on each code pushes to the **main**, **develop**
   branches or on submitting Pull Request to the **main** branch
3. At each launch, the build status of the project is checked and Unit tests are run
   </br></br>![workflows](https://github.com/AndreyTokmakov/PricePointBook/blob/metadata/images/actions_1_workflows.png)
   </br></br>![Unit-tests status](https://github.com/AndreyTokmakov/PricePointBook/blob/metadata/images/actions_2_unit_tests.png)
   </br></br>![Pipeline1](https://github.com/AndreyTokmakov/PricePointBook/blob/metadata/images/actions_3_pipeline.png)
   </br></br>![Pipeline2](https://github.com/AndreyTokmakov/PricePointBook/blob/metadata/images/actions_3_pipeline_2.png)

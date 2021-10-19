
## Adding Options

1. Add new option with its default value to `default_config` dictionary in `config.py`.
2. Introduce a corresponding macro in `build_system.py`
The option will be automatically added to the `config.json` file during the next compilation.



## Adding a Feature Line

1. Add the feature line header with atleast a `void setup()` and `void loop()` to the `./src` subdirectory.
2. Add the option flag in the `All Features` list in the `json.config` file. (Also add it in the `default_config` dictionary in `config.py`)
3. If the index of the option flag in the list is `N`. Use the `#if DEVICE_FEATURELINE == N` enabler in the `int main()` function.
---
4. For the PC Software, specific handling is required for any feature line. The abstract `class FLMode` can be inherited to provide base functions to any mode object. It provides the necessary functionality needed for a particular measurement. This includes creating files for saving data, creating global resource, provide a function for parsing the received `struct` to data files and plotting interface, and generating a `struct` representation.





## Design Goal

Complete Context Parsar → understands each single micro-mode. Each micro-mode either generates an action or an error. Moreover, each micro-mode is part of a hierarchy of modules, and is part of the module it applies on.

measures('CountRate':[line graph, scrolling, deque100, nofile]) → creates the necessary functionality needed for a particular measurement. 

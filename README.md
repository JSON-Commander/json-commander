Commander
=========
## JSON base command line interface definitions for C and C++ applications

### Overview
Commander provides functionality for defining a command line interface for applications based on a command line schema in the form of a JSON specification.  Commander parses an application's command line arguments, together with the environment and the system, user and local configuration files to produce JSON data that is provided as the runtime configuration for an application.  Commander provides tools to validate application schemas, generate JSON schemas specifying the data that will be provided to the target application and to generate man pages.  Additionally, commander provides an application to that will parse arguments given a path to the command line schema and output the corresponding runtime configuration in JSON.

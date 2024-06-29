# MARTY Plugins

`MARTY` plugins are additional libraries that rely on `MARTY` but are shipped in separate libraries.
The requirements are the following:

 - A `MARTY` plugin should have its **own directory** in `marty-plugins/`
 - A `MARTY` plugin should generate its **own libraries**, static **and** shared
 - A `MARTY` plugin can depend on `MARTY`, the contrary should never be true i.e. `MARTY` cannot depend on a plugin
 - A `MARTY` plugin should be tested automatically
# CharsetLoader

This folder contains :
* an arduino example (`CharsetLoader.ino`) to load charset into MAX7456
* a script (`maxcharset`) to manipulate and convert image to MAX7456 charset

# Usage

Compile and upload example, then connect using serial monitor and answer yes to write charset to MAX7456.

This will send the charset described by `charset.h`.

# Charset manipulation tool

This `charset.h` can be manipulated by `maxcharset`, with `maxcharset` you can:
* create a blank charset
* convert a charset to C (e.g. `charset.h` used in this example)
* check a charset (ie. show pixel errors)
* clean a charset (ie. remove not-fully black or white pixels and not-fully transparent or opaque)
* import a symbol into charset (e.g. import an image that takes 3x2 characters area)
* import symbols into charset (e.g. big digits set)

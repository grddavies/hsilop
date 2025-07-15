# hsilop

Reverse Polish Notation calculator

> There are other, more featureful RPN calculators, but I wrote this one.

## Build

Requires `readline`.

```sh
make
# creates: build/main
```

## Run

Read RPN expressions from stdin:

```sh
echo "-10.2 2 / 1e6 *" | ./build/main
# -5.1e+06
```

Or use in interactive mode, with basic readline support:

```sh
./build/main
```

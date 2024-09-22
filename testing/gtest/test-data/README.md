# Test Data Directory

This directory contains test files formatted as ascii text (.txtpb) or binary (.binpb).  The `Protoscope` tool is leveraged to turn the text based protobuf
files into their corresponding `binpb` binary data that can then be used to exercise the `spicy-protobuf` parsing capabilities. For now, this is done manually,
so any updates to the `txtpb` files will not propagate to `binpb` files automatically.

See the `Protoscope` installation instructions located at the reference below.

Command:

```bash
# Generate a .binpb from a .txtpb
protoscope -s <test-file.txtpb> > <test-file.binpb>
```

Example text based format (.txtpb):

```txt
field_num: value
1: 150
```

To more easily view the contents of a .binpb file:

```bash
hexdump -C <test-file.binpb>
```

To feed a .binpb file into the generated spicy code:

```bash
spicy-driver <*.hlto> -f <test-file.binpb>
```

See the `Protoscope` [test data](https://github.com/protocolbuffers/protoscope/tree/main/testdata) directory for additional examples.  Specifically, the `*.golden` files.

## References

* [Protoscope](https://github.com/protocolbuffers/protoscope)


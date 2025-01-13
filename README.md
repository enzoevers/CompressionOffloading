# CompressionOffloading
> Project board: https://github.com/users/enzoevers/projects/2

Compress and decompress data with common algorithms and use benchmarks to see if offloading compression and/or decompression for large data sets can improve performance.

As for now, first the [DEFLATE](https://en.wikipedia.org/wiki/Deflate) algorithm will be used with the [ZIP](https://en.wikipedia.org/wiki/ZIP_(file_format)) file format.

## Personal goals
- Learn about (de)compression algorithms
- Get more experience with FPGA/Zynq

## Development environment setup
See [DevEnvSetup.md](./DevEnvSetup.md).

Note that all development should keep the following targets in mind:
- x86 Linux | Windows | MacOS
- ARM MacOS
- ARM Zynq

## Plan
1. Use the [zlib library](https://www.zlib.net/) to compress and decompress ([example](https://zlib.net/zlib_how.html)). This should run on both Deskptop and the Zynq Processing System (PS).
    - A single file
        - [x] DEFLATE -> compressed data file
        - [ ] DEFLATE -> ZIP file
        - [x] compressed data file -> INFLATE
        - [x] ZIP file -> INFLATE
    - Multiple files
        - [ ] DEFLATE -> ZIP file
        - [x] ZIP file -> INFLATE
    - For benchmarking see: https://github.com/okuvshynov/b63
2. Write the DEFLATE algorithm in C.
    - [ ] See `1.`, but replace the DEFLATE implementation from zlib with my custom implementation
3. Write the INFLATE algorithm in C.
    - [ ] See `1.`, but replace the INFLATE implementation from zlib with my custom implementation
4. Create a pipeline on the Zynq that sends data from the Processing System (PS) to the FPGA/Programmable Logic (PL) and back to the PS.
    - [ ] Success
6. Implement an FPGA-friendly version of DEFLATE in VHDL
    - [ ] See `1.`, but replace the DEFLATE implementation from zlib with my custom FPGA-offloading implementation
7. Implement an FPGA-friendly version of INFLATE in VHDL
    - [ ] See `1.`, but replace the DEFLATE implementation from zlib with my custom FPGA-offloading implementation

Now compression and decompression works. Something that I was thinking about as well was that a stream of incoming data in the FPGA could be compressed on the FPGA and, as a stream, send compressed chucks to the PS. This way it may be possible to store data (logging) data more efficiently without needing a lot of RAM.

## References and useful links
### (De)comporession algorithm
- https://en.wikipedia.org/wiki/Deflate
    - Also see the "Hardware encoders" section
- https://zlib.net/feldspar.html
- https://www.infinitepartitions.com/cgi-bin/showarticle.cgi?article=art001
- https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-wusp/fb98aa28-5cd7-407f-8869-a6cef1ff1ccb?redirectedfrom=MSDN

### FPGA (de)compression algorithm related
- https://github.com/tomtor/HDL-deflate
- https://www.cast-inc.com/compression/lossless-data-compression/zipaccel-c + https://www.cast-inc.com/company/discontinued-not-categorized/gzip-rd-xil
- https://www.cast-inc.com/compression/lossless-data-compression/zipaccel-d

### Generic FPGA related
- https://github.com/TripRichert/viv-prj-gen

### Benchmarking
- https://github.com/okuvshynov/b63

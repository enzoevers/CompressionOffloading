# CompressionOffloading
Compress and decompress data with common algorithms and use benchmarks to see if offloading compression and/or decompression for large data sets can improve performance.

As for now, first the [DEFLATE](https://en.wikipedia.org/wiki/Deflate) algorithm will be used with the [ZIP](https://en.wikipedia.org/wiki/ZIP_(file_format)) file format.

## Personal goals
- Learn about (de)compression algorithms
- Get more experience with FPGA/Zynq

## Plan
0. Use the [zlib library](https://www.zlib.net/) to compress and decompress ([example](https://zlib.net/zlib_how.html)):
    - [ ] A C-string
    - [ ] A single file
        - [ ] DEFLATE
        - [ ] DEFLATE -> ZIP
        - [ ] INFLATE
        - [ ] ZIP -> INFLATE
    - [ ] Multiple files
        - [ ] DEFLATE -> ZIP
        - [ ] ZIP -> INFLATE
1. Write the DEFLATE algorithm in C.
    - [ ] See `0.` but replace the DEFLATE implementation from zlib with my custom implementation
2. Write the INFLATE algorithm in C.
    - [ ] See `0.` but replace the DEFLATE implementation from zlib with my custom implementation

These first points should give a good understanding of the DEFLATE and INFLATE algorithms and ZIP format.

To recall my knowledge of FPGA development (VHDL) first a simple infrastructure will be created.

3. In the Zynq device, implement infrastructure that can send a file from the Processing System (PS) (in chunks) to the Programmable Logic (PL), and where the PL sends de data back to the PS.
    - [ ] Send a C-string from PS -> PL -> PS
4. Find out how to profile the individual parts of pipeline to compare performance numbers.
5. Implement an FPGA-friendly version of DEFLATE in VHDL
6. Implement an FPGA-friendly version of INFLATE in VHDL

Now compression and decompression work. Something that I was thinking about as well was that a stream of incoming data in the FPGA could be compressed on the FPGA and, as a stream, send compressed chucks to the PS. This way it may be possible to store data (logging) data more efficiently without needing a lot of RAM.

## References
- https://en.wikipedia.org/wiki/Deflate
    - Also see the "Hardware encoders" section
- https://github.com/tomtor/HDL-deflate
- https://www.cast-inc.com/compression/lossless-data-compression/zipaccel-c + https://www.cast-inc.com/company/discontinued-not-categorized/gzip-rd-xil
- https://www.cast-inc.com/compression/lossless-data-compression/zipaccel-d
- https://github.com/TripRichert/viv-prj-gen
- https://zlib.net/feldspar.html
- https://www.infinitepartitions.com/cgi-bin/showarticle.cgi?article=art001
- https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-wusp/fb98aa28-5cd7-407f-8869-a6cef1ff1ccb?redirectedfrom=MSDN

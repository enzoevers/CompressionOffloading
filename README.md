# CompressionOffloading
Compress and decompress data with common algorithms and use benchmarks to see if offloading compression and/or decompression for large data sets can improve performance.

As for now, first the [DEFLATE](https://en.wikipedia.org/wiki/Deflate) algorithm will be used with the [ZIP](https://en.wikipedia.org/wiki/ZIP_(file_format)) file format.

## Personal goals
- Learn about compression algorithms
- Get more experience with FPGA/Zynq (the platform to offload to)

## Plan
1. Write the DEFLATE compression algorithm in C.
    - [ ] Successful if existing decompression programs can decompress the data that was compressed with my custom C algorithm.
2. Write a decompression algorithm in C.
    - [ ] Successful if my custom C decompression algorithm can decompress the data that was compressed with existing compression programs.

These first two points should give me a good understanding of the chosen (de)compression algorithm(s).

To recall my knowledge of FPGA development (VHDL) first a simple infrastructure will be created.

3. In the Zynq device, implement infrastructure that can send a file from the Processing System (PS) (in chunks) to the Programmable Logic (PL), and where the PL sends de data back to the PS.
    - [ ] Files of any extension can be sent via the PS -> PL -> PS pipeline and the received file is identical to the sent file.
4. Find out how to profile the individual parts of pipeline to compare performance numbers.
5. Implement the compression algorithm on the FPGA.
6. Implement the decompression algorithm on the FPGA.

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

#include <CoDeLib/Deflate_zlib/Deflate_zlib.h>

#include <assert.h>
#include <stdio.h>
#include <zlib.h>

/*
 * Based on https://www.zlib.net/zlib_how.html
 */

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#include <fcntl.h>
#include <io.h>
#define SET_BINARY_MODE(file) _setmode(_fileno(file), O_BINARY)
#else
#define SET_BINARY_MODE(file)
#endif

#define BUFFER_SIZE_BYTES 16384

DEFLATE_RETURN_CODES Deflate(FILE *pInputFile, FILE *pOutputFile,
                             void *pOptions __attribute__((unused))) {
    if (pInputFile == NULL || pOutputFile == NULL) {
        printf("Invalid input or output file\n");
        return DEFLATE_ERROR;
    }

    int returnCode;
    int flushState;
    unsigned totalDataReturned;
    z_stream tmpDataStream;
    unsigned char inBuffer[BUFFER_SIZE_BYTES];
    unsigned char outBuffer[BUFFER_SIZE_BYTES];

    int method = Z_DEFLATED;
    int windowBits = -9;
    int memLevel = 8;
    int strategy = Z_DEFAULT_STRATEGY;

    int compressionLevel = Z_DEFAULT_COMPRESSION;

    /* allocate deflate state */
    tmpDataStream.zalloc = Z_NULL;
    tmpDataStream.zfree = Z_NULL;
    tmpDataStream.opaque = Z_NULL;
    returnCode = deflateInit2(&tmpDataStream, compressionLevel, method,
                              windowBits, memLevel, strategy);
    if (returnCode != Z_OK) {
        return returnCode;
    }

    /* compress until end of file */
    do {
        tmpDataStream.avail_in =
            fread(inBuffer, 1, BUFFER_SIZE_BYTES, pInputFile);
        if (ferror(pInputFile)) {
            printf("Error reading input file\n");
            (void)deflateEnd(&tmpDataStream);
            return DEFLATE_ERROR;
        }
        flushState = feof(pInputFile) ? Z_FINISH : Z_NO_FLUSH;
        tmpDataStream.next_in = inBuffer;

        /* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do {
            tmpDataStream.avail_out = BUFFER_SIZE_BYTES;
            tmpDataStream.next_out = outBuffer;

            returnCode =
                deflate(&tmpDataStream, flushState); /* no bad return value */
            assert(returnCode != Z_STREAM_ERROR);    /* state not clobbered */

            totalDataReturned = BUFFER_SIZE_BYTES - tmpDataStream.avail_out;
            if (fwrite(outBuffer, 1, totalDataReturned, pOutputFile) !=
                    totalDataReturned ||
                ferror(pOutputFile)) {
                printf("Error writing output file\n");
                (void)deflateEnd(&tmpDataStream);
                return Z_ERRNO;
            }

        } while (tmpDataStream.avail_out == 0);
        assert(tmpDataStream.avail_in == 0); /* all input will be used */

        /* done when last data in file processed */
    } while (flushState != Z_FINISH);
    assert(returnCode == Z_STREAM_END); /* stream will be complete */

    /* clean up and return */
    (void)deflateEnd(&tmpDataStream);

    return DEFLATE_SUCCESS;
}

const struct IDeflate deflate_zlib = {
    .Deflate = Deflate,
};

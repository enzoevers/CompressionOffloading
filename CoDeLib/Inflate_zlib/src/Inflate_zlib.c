#include <CoDeLib/Inflate_zlib/Inflate_zlib.h>

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

INFLATE_RETURN_CODES Inflate(FILE *pInputFile, FILE *pOutputFile,
                             void *pOptions __attribute__((unused))) {
    if (pInputFile == NULL || pOutputFile == NULL) {
        printf("Invalid input or output file\n");
        return INFLATE_ERROR;
    }

    int returnCode;
    unsigned totalDataReturned;
    z_stream tmpDataStream;
    unsigned char inBuffer[BUFFER_SIZE_BYTES];
    unsigned char outBuffer[BUFFER_SIZE_BYTES];

    int windowBits = -9;

    /* allocate inflate state */
    tmpDataStream.zalloc = Z_NULL;
    tmpDataStream.zfree = Z_NULL;
    tmpDataStream.opaque = Z_NULL;
    tmpDataStream.avail_in = 0;
    tmpDataStream.next_in = Z_NULL;
    returnCode = inflateInit2(&tmpDataStream, windowBits);
    if (returnCode != Z_OK) {
        return INFLATE_ERROR;
    }

    /* decompress until deflate stream ends or end of file */
    do {
        tmpDataStream.avail_in =
            fread(inBuffer, 1, BUFFER_SIZE_BYTES, pInputFile);
        if (ferror(pInputFile)) {
            (void)inflateEnd(&tmpDataStream);
            return INFLATE_ERROR;
        }
        if (tmpDataStream.avail_in == 0) {
            break;
        }
        tmpDataStream.next_in = inBuffer;

        /* run inflate() on input until output buffer not full */
        do {
            tmpDataStream.avail_out = BUFFER_SIZE_BYTES;
            tmpDataStream.next_out = outBuffer;

            returnCode = inflate(&tmpDataStream, Z_NO_FLUSH);
            assert(returnCode != Z_STREAM_ERROR); /* state not clobbered */
            switch (returnCode) {
            case Z_NEED_DICT: {
                returnCode = Z_DATA_ERROR; /* and fall through */
                __attribute__((fallthrough));
            }
            case Z_DATA_ERROR:
            case Z_MEM_ERROR: {
                (void)inflateEnd(&tmpDataStream);
                return INFLATE_ERROR;
            }
            }

            totalDataReturned = BUFFER_SIZE_BYTES - tmpDataStream.avail_out;
            if (fwrite(outBuffer, 1, totalDataReturned, pOutputFile) !=
                    totalDataReturned ||
                ferror(pOutputFile)) {
                (void)inflateEnd(&tmpDataStream);
                return INFLATE_ERROR;
            }

        } while (tmpDataStream.avail_out == 0);

        /* done when inflate() says it's done */
    } while (returnCode != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&tmpDataStream);
    return returnCode == Z_STREAM_END ? INFLATE_SUCCESS : INFLATE_ERROR;
}

const struct IInflate inflate_zlib = {
    .Inflate = Inflate,
};

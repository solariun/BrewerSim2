/**
 * @file BMPConverter.cpp
 * @author      Gustavo Campos (www.github.com/solariun)
 * @brief       Converts BMP to C/c++ bitmap array
 * @version     0.1
 * @date        2020-10-26
 *
 * @copyright Copyright (c) 2020 Gustavo Campos

    THE BMP FILE FORMAT
    Compiled by Nathan Liesch of Imperium Accelero 9000


    Increasingly the power of FPGAs is being utilized for DSP applications and a common source for digital signals to process is images. The
    first step in implementing any sort of image processing algorithm is accessing the raw pixel data.

    The MS-Windows standard format is BMP and was developed as a device-independent bitmap (DIB) format that will allow Windows to
    display the bitmap on any type of display device. The term "device independent" means that the bitmap specifies pixel color in a form
    independent of the method used by a display to represent color. This file format can be stored uncompressed, so reading BMP files is
    fairly simple; most other graphics formats are compressed, and some, like GIF, are difficult to decompress.

    The file format consists of the following structures:
                Corresponding
    Structure                    Description
                Bytes
    Header     0x00 - 0x0D              contains information about the type, size, and layout of a device-independent bitmap file
    InfoHeader 0x0E - 0x35              specifies the dimensions, compression type, and color format for the bitmap
                                        contains as many elements as there are colors in the bitmap, but is not present for bitmaps with 24 color
    ColorTable 0x36 - variable          bits because each pixel is represented by 24-bit red-green-blue (RGB) values in the actual bitmap data
                                        area
                                        an array of bytes that defines the bitmap bits. These are the actual image data, represented by
                                        consecutive rows, or "scan lines," of the bitmap. Each scan line consists of consecutive bytes
    Pixel Data variable
                                        representing the pixels in the scan line, in left-to-right order. The system maps pixels beginning with the
                                        bottom scan line of the rectangular region and ending with the top scan line.

    Below is a more detailed table of the contents of each of these structures.
    _______________________________________________________________
    Header   14 bytes  Windows Structure: BITMAPFILEHEADER
    _______________________________________________________________

    Name           Size                      Offset  Description

    Signature      2 bytes                    0000h 'BM'

    FileSize       4 bytes                    0002h  File size in bytes

    reserved       4 bytes                    0006h  unused (=0)

    DataOffset     4 bytes                    000Ah  Offset from beginning of file to the beginning of the bitmap data

    _______________________________________________________________
    InfoHeader     40 bytes    Windows Structure: BITMAPINFOHEADER
    _______________________________________________________________

    Size           4 bytes                    000Eh    Size of InfoHeader =40

    Width          4 bytes                    0012h    Horizontal width of bitmap in pixels

    Height         4 bytes                    0016h    Vertical height of bitmap in pixels

    Planes         2 bytes                    001Ah    Number of Planes (=1)

    Bits Per Pixel 2 bytes                    001Ch Bits per Pixel used to store palette entry information. This also identifies in an indirect way
                                                    the number of possible colors. Possible values are:
                                                        1 = monochrome palette. NumColors = 1
                                                        4 = 4bit Palletaized. NumColors = 16
                                                        8 = 8bit Palletaized. NumColors = 256
                                                        16 = 16bit RGB. NumColors = 65536
                                                        24 = 24bit RGB. NumColors = 16M

    Compression 4 bytes                       001Eh    Type of Compression
                                                        0 = BI_RGB no compression
                                                        1 = BI_RLE8 8bit RLE encoding
                                                        2 = BI_RLE4 4bit RLE encoding

    ImageSize      4 bytes                    0022h    (compressed) Size of Image
                                                        It is valid to set this =0 if Compression = 0

    XpixelsPerM 4 bytes                       0026h    horizontal resolution: Pixels/meter

    YpixelsPerM 4 bytes                       002Ah    vertical resolution: Pixels/meter

    Colors Used 4 bytes                       002Eh    Number of actually used colors. For a 8-bit / pixel bitmap this will be 100h or 256.

    Important      4 bytes                    0032h    Number of important colors

    Colors                                             0 = all

    _______________________________________________________________
    ColorTable        4 * NumColors bytes        0036h    present only if Info.BitsPerPixel less than 8
    _______________________________________________________________
                                                        colors should be ordered by importance
        Red           1 byte                              Red intensity

        Green         1 byte                              Green intensity

        Blue          1 byte                              Blue intensity
        reserved      1 byte                            unused (=0)

    repeated NumColors times


    Pixel Data        InfoHeader.ImageSize              The image data
                    bytes


    Bits Per Pixel Field

    Value Description
        The bitmap is monochrome, and the palette contains two entries. Each bit in the bitmap array represents a pixel. If the bit is clear,
    1      the pixel is displayed with the color of the first entry in the palette; if the bit is set, the pixel has the color of the second entry in
 the table.

        The bitmap has a maximum of 16 colors, and the palette contains up to 16 entries. Each pixel in the bitmap is represented by a 4-
    4      bit index into the palette. For example, if the first byte in the bitmap is 1Fh, the byte represents two pixels. The first pixel contains
        the color in the second palette entry, and the second pixel contains the color in the sixteenth palette entry.
        The bitmap has a maximum of 256 colors, and the palette contains up to 256 entries. In this case, each byte in the array
    8      represents a single pixel.

        The bitmap has a maximum of 2^16 colors. If the Compression field of the bitmap file is set to BI_RGB, the Palette field does not
        contain any entries. Each word in the bitmap array represents a single pixel. The relative intensities of red, green, and blue are
        represented with 5 bits for each color component. The value for blue is in the least significant 5 bits, followed by 5 bits each for
    16     green and red, respectively. The most significant bit is not used.
        If the Compression field of the bitmap file is set to BI_BITFIELDS, thePalette field contains three 4 byte color masks that specify
        the red, green, and blue components, respectively, of each pixel. Each 2 bytes in the bitmap array represents a single pixel.

    24     The bitmap has a maximum of 2^24 colors, and the Palette field does not contain any entries. Each 3-byte triplet in the bitmap
        array represents the relative intensities of blue, green, and red, respectively, for a pixel.

    Additional Info

    Each scan line is zero padded to the nearest 4-byte boundary. If the image has a width that is not divisible by four, say, 21 bytes, there
    would be 3 bytes of padding at the end of every scan line.

    Scan lines are stored bottom to top instead of top to bottom.

    RGB values are stored backwards i.e. BGR.

    4 bit & 8 bit BMPs can be compressed. BMPs use a very simple form of compression called Run Length Encoded (RLE). Instead of
    storing a value for each pixel RLE stores a number, N, followed by an index. This means that the next N pixels are of the color for this
    index.

    For additional information refer to:
    MSDN Library: Bitmap Storage
        http://msdn.microsoft.com/library/default.asp?url=/library/en-us/gdi/bitmaps_4v1h.asp
    The Graphics File Formats Page - BMP
        http://www.dcs.ed.ac.uk/home/mxr/gfx/2d/BMP.txt


    SOURCE: http://www.ece.ualberta.ca/~elliott/ee552/studentAppNotes/2003_w/misc/bmp_file_format/bmp_file_format.htm
 */

#include <fcntl.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdint>
#include <iostream>
#include <string>

uint8_t nRGBStruct[][3] = {
        {3, 2, 3},  // 8 bits
        {5, 6, 5},  // 16 bits
        {8, 8, 8}   // 32 bits
};
/**
 * @brief   VERIFY a expression
 *
 * @param   x   Expression
 * @param   m   Message
 * @param   r   Return
 *
 * @note    This will only work inside functions.
 */
#define VERIFY(x, m, r)                                                                                          \
    if (!(x))                                                                                                    \
    {                                                                                                            \
        std::cout << __FILE__ << ":" << __FUNCTION__ << "(" << __LINE__ << ")[" << #x << "]:" << m << std::endl; \
        return r;                                                                                                \
    }

/**
 * @brief   VERIFYFALSE a expression
 *
 * @param   x   Expression
 * @param   m   Message
 *
 * @note    This will only work inside functions.
 */
#define VERIFYFALSE(x, m) VERIFY (x, m, false)

// Return default error message for C std functions
#define CERRORMSG strerror (errno)

// Print a line on stdout
#define LINE() std::cout << "----------------------------------------------" << std::endl;

// BMP related constants

// The BMP signature for BM
#define BMP_SIGNATURE 19778

// Compression method accepted
#define BMP_BI_RGB 0

// Accepted Bitmap header size
#define BMP_BITMAPINFOHEADER 40

// BMP header bytes size
#define BMP_HEADERSIZE 14
/**
 * @brief   Execute Ceilling calculation
 *          to next integer available
 *
 * @param   nValue a value to be use as reference
 *
 * @return double  return the next integer from the reference
 */
double RoundUp (double nValue)
{
    uint64_t nFloorValue = (uint64_t)nValue;

    return (double)nValue == nFloorValue ? nFloorValue : nFloorValue + 1;
}

#define ABS(x) (x < 0 ? (x * (-1)) : x)

/**
 * @brief   PrintBinary
 *
 * @param pBuffer   The buffer to be printed
 * @param nSize     The size of the buffer
 */
void PrintBinary (const uint8_t* pBuffer, size_t nSize)
{
    size_t nCount = 0;
    uint8_t nCountb = 0;
    uint8_t nValue = 0;

    for (nCount = 0; nCount < nSize; nCount++)
    {
        nValue = pBuffer[nCount];

        nCountb = 0;

        do
        {
            // printf (" nCountb: [%u] - [%u]\n", nCountb, (1 << nCountb));

            if ((nValue & (1 << (7 - nCountb))) != 0)
                std::cout << "1";
            else
                std::cout << "0";

        } while (++nCountb < 8);
    }
}

/**
 * @brief   Default RGB structure
 *
 */
struct RGB
{
    uint8_t nRed;
    uint8_t nGreen;
    uint8_t nBlue;
    uint8_t nAlpha;
} __attribute__ ((__packed__));

class ImageBuffer
{
public:
    ImageBuffer() = delete;

    ImageBuffer (uint32_t nWidth, int32_t nHeight) : m_nWidth(nWidth), m_nHeight(nHeight), m_ImageBytesSize(0), m_RGBBuffer(nullptr)
    {
        m_ImageBytesSize = (nWidth * nHeight) * sizeof (RGB);
        m_RGBBuffer = new RGB[nWidth * nHeight];
    }

private:
    // The Width of the buffered image
    uint32_t m_nWidth;

    // The Height of the buffered image
    uint32_t m_nHeight;

    // The Size in bytes of the image
    size_t m_ImageBytesSize;

    //RGB Buffer with the image
    RGB* m_RGBBuffer;
}

/**
 * @brief   Load BMP file into memory
 */
class BMP
{
public:
    BMP () : m_bmpHeader (), m_nFD (0), m_pRGBData (nullptr), m_pRGBPalleta (nullptr)
    {
        memset ((void*)&m_bmpHeader, 0, sizeof (BMPHeader));
    }

    bool Load (const std::string& strFileName)
    {
        VERIFYFALSE (strFileName.length () != 0, "Filename has zero length()");

        memset ((void*)&m_bmpHeader, 0, sizeof (BMPHeader));

        VERIFYFALSE ((m_nFD = open (strFileName.c_str (), O_RDONLY)) > 0, "Error, file " << strFileName << " could not be opened: " << CERRORMSG);

        VERIFYFALSE (read (m_nFD, (void*)&m_bmpHeader, sizeof (BMPHeader)) >= 0, "Error reading header: " << CERRORMSG);

        std::cout << "Struct size       : " << sizeof (m_bmpHeader) << std::endl;
        std::cout << "signature         : [" << ((char*)&m_bmpHeader.signature)[0] << "][" << ((char*)&m_bmpHeader.signature)[1] << "] - "
                  << m_bmpHeader.signature << std::endl;
        std::cout << "Bitmap Data offset: " << m_bmpHeader.nDataOffSet << std::endl;
        std::cout << "nRawBitmapSize    : " << m_bmpHeader.nRawBitmapSize << std::endl;
        std::cout << "nBitmapByteSize   : " << m_bmpHeader.nBitmapByteSize << ", Row size: " << (m_bmpHeader.nBitmapByteSize / m_bmpHeader.nHeight)
                  << std::endl;
        std::cout << "nWidth            : " << m_bmpHeader.nWidth << std::endl;
        std::cout << "nHeight           : " << m_bmpHeader.nHeight << std::endl;
        std::cout << "nBitPerPixel      : " << m_bmpHeader.nBitPerPixel << std::endl;
        std::cout << "nCompressionMethod: " << m_bmpHeader.nCompressionMethod << std::endl;
        std::cout << "nBIDHeaderSize    : " << m_bmpHeader.nBIDHeaderSize << std::endl;
        std::cout << "nColorsInPalleta  : " << m_bmpHeader.nColorsInPalleta << std::endl;
        std::cout << "nImportantColors  : " << m_bmpHeader.nImportantColors << std::endl;
        LINE ();

        // Only Signature BM (Windows) and BITMAPINFOHEADER (40 bytes BID) is supported
        VERIFYFALSE (
                m_bmpHeader.signature == BMP_SIGNATURE,
                "Signature value [" << ((char*)&m_bmpHeader.signature)[0] << "][" << ((char*)&m_bmpHeader.signature)[1] << "], is not implemented.");
        VERIFYFALSE (m_bmpHeader.nCompressionMethod == BMP_BI_RGB,
                     "Error, compression [" << m_bmpHeader.nCompressionMethod << "] is not implemented.");

        LoadColorPalleta ();

        LoadRGBData ();

        close (m_nFD);

        return true;
    }

private:
    struct BMPHeader
    {
        // File Header
        uint16_t signature;
        uint32_t nBitmapByteSize;
        uint32_t nReserved;
        uint32_t nDataOffSet;

        // BID Header
        uint32_t nBIDHeaderSize;
        int32_t nWidth;
        int32_t nHeight;
        uint16_t nColorPanes;
        uint16_t nBitPerPixel;
        uint32_t nCompressionMethod;
        uint32_t nRawBitmapSize;
        uint32_t nWidthPixPerMeter;
        uint32_t nHeightPixPerMeter;
        uint32_t nColorsInPalleta;
        uint32_t nImportantColors;
    } __attribute__ ((__packed__));

    BMPHeader m_bmpHeader;
    int m_nFD;

    RGB* m_pRGBPalleta;
    RGB* m_pRGBData;

    bool LoadColorPalleta ()
    {
        VERIFYFALSE (m_bmpHeader.signature == BMP_SIGNATURE, "No image was loaded.");

        if (m_bmpHeader.nColorsInPalleta > 0)
        {
            int nPalleteOffset = BMP_HEADERSIZE + m_bmpHeader.nBIDHeaderSize;

            std::cout << "Pallet Offset : " << std::hex << nPalleteOffset << std::dec << std::endl;

            VERIFYFALSE (lseek (m_nFD, nPalleteOffset, SEEK_SET) > 0, "Error Accessing Color Palleta area [" << nPalleteOffset << "[, " << CERRORMSG);

            if (m_pRGBPalleta != nullptr)
            {
                delete[](m_pRGBPalleta);
            }

            m_pRGBPalleta = new RGB[m_bmpHeader.nColorsInPalleta];

            for (int nCount = 0; nCount < m_bmpHeader.nColorsInPalleta; nCount++)
            {
                VERIFYFALSE (read (m_nFD, (void*)&m_pRGBPalleta[nCount], sizeof (RGB)) > 0,
                             "Error reading index " << nCount << " Palleta, " << CERRORMSG);
                std::cout << nCount << "\t"
                          << " R:[" << (int)m_pRGBPalleta[nCount].nRed << "], G:[" << (int)m_pRGBPalleta[nCount].nGreen << "], B: ["
                          << (int)m_pRGBPalleta[nCount].nBlue << "], A:[" << (int)m_pRGBPalleta[nCount].nAlpha << "]" << std::endl;
            }
        }

        LINE ();
        return true;
    }

    bool LoadRGBData ()
    {
        VERIFYFALSE (m_bmpHeader.signature == BMP_SIGNATURE, "No image was loaded.");

        double nBytesPerPixelFactor = (double)m_bmpHeader.nBitPerPixel / 8;
        uint8_t nBytesPerPixel = nBytesPerPixelFactor < 1 ? 1 : nBytesPerPixelFactor;
        uint32_t nBytesPerRow = (uint32_t)RoundUp ((double)m_bmpHeader.nWidth * nBytesPerPixelFactor);
        uint8_t nPaddingSize = nBytesPerRow % 4 > 0 ? (4 - nBytesPerRow % 4) : 0;
        uint32_t nRowDataSize = nBytesPerRow + nPaddingSize;

        uint32_t nDataProcessing;

        off_t nReturn;

        std::cout << "ByP/Pixel factor  : " << nBytesPerPixelFactor << std::endl;
        std::cout << "Bytes per Pixel   : " << (int)nBytesPerPixel << std::endl;
        std::cout << "Bytes per Row     : " << nBytesPerRow << std::endl;
        std::cout << "Row Data Size     : " << nRowDataSize << std::endl;
        std::cout << "Row Padding Size  : " << (int)nPaddingSize << std::endl;
        LINE ();

        for (uint32_t nRowCount = 0; nRowCount < ABS (m_bmpHeader.nHeight); nRowCount++)
        {
            VERIFYFALSE ((nReturn = lseek (m_nFD, m_bmpHeader.nDataOffSet + (nRowDataSize * nRowCount), SEEK_SET)) > 0,
                         "Error setting next row data location, Row size [" << nRowDataSize << "], return: [" << nReturn << "] " << CERRORMSG);
            std::cout << std::dec << nRowCount << ":lseek: " << nReturn << ": ";

            for (uint32_t nCount = 0; nCount < nBytesPerRow; nCount++)
            {
                nDataProcessing = 0;
                VERIFYFALSE (read (m_nFD, (void*)&nDataProcessing, nBytesPerPixel) >= 0,
                             "Error reading [" << nBytesPerPixel << "] bytes data file, " << CERRORMSG);

                PrintBinary ((uint8_t*)&nDataProcessing, nBytesPerPixel);
                // std::cout << std::hex << "(" << nDataProcessing << ")";

                if (m_bmpHeader.nBitPerPixel >= 8)
                {
                }
            }

            std::cout << std::endl;
        }

        return true;
    }
};

int main (int nArgc, char** ppszArgs)
{
    if (nArgc != 2)
    {
        std::cerr << "Error, give a filename" << std::endl;
        exit (1);
    }

    BMP bmp;

    bmp.Load (ppszArgs[1]);

    exit (0);
}

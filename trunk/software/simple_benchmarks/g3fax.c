/******* PowerStone Benchmark *******/

#include <math.h>
#include <stdio.h>

#define MAXCOLS     1728
#define MAXROWS     2150


static  int     eof = 0;
static  unsigned long   sum = 0;
static  int     eol = 0;
static  unsigned char   rowbuf[MAXCOLS];


static  unsigned long   faxsum = 11296; /* number of black pixels in image */

unsigned char   fax[] =
{
    0x49,0x49,0x2a,0x00,0x93,0xa1,0x00,0x00,0x00,0x80,0xb2,0x59,0x01,0x00,0x80,0xb2,
        0x59,0x01,0x00,0x80,0xb2,0x59,0x01,0x00,0x80,0xb2,0x59,0x01,0x00,0x80,0xa9,0x9a,
        0x7f,0x74,0xf3,0x15,0x67,0x70,0xc5,0x1c,0x72,0x10,0x3e,0xe8,0xf2,0x81,0x6f,0x70,
        0x64,0x70,0x87,0x1f,0xa4,0xe7,0x2b,0x43,0x0e,0x7e,0x90,0x83,0x34,0x38,0x02,0x92,
        0xd1,0x9e,0xef,0xf3,0x1d,0xdc,0xc1,0xe5,0xf0,0x83,0xf4,0x7c,0x3f,0x81,0x82,0x37,
        0x44,0x27,0x85,0x34,0xb8,0x83,0x1f,0x7a,0x90,0x8c,0x36,0x78,0x43,0x76,0xc3,0xc1,
        0x10,0xbe,0x9e,0x01,0x00,0x80,0xa9,0xf2,0xb9,0xbf,0x7b,0x3e,0xd8,0x88,0xc1,0xce,
        0xf3,0x7d,0xfe,0x99,0xd6,0x95,0xb3,0x2b,0xf7,0xa0,0x2f,0xf3,0x7d,0xbe,0xfb,0xac,
        0xfa,0xdd,0xf3,0x7d,0xbe,0xcf,0xcf,0x2b,0xe6,0xf7,0x79,0x9f,0xef,0xf3,0xf3,0x88,
        0xe5,0xf3,0x7d,0xbe,0xcf,0xf7,0x93,0xea,0x79,0xbe,0xae,0x3c,0xdf,0x5f,0xe6,0xf7,
        0xc4,0xfc,0x9e,0x98,0xef,0xf3,0xb7,0xcf,0x97,0xcf,0x3f,0xeb,0x19,0x00,0x80,0xa9,
        0xda,0xf7,0x7d,0x9f,0xdf,0xdf,0xe0,0x7d,0xf9,0xe0,0xfd,0x3c,0xdf,0xe7,0xf7,0x3e,
        0xf5,0xfe,0xb2,0x9f,0xbd,0xec,0xf7,0xa0,0x2f,0xf3,0x7d,0xbe,0xef,0xfb,0xbe,0xea,
        0x79,0x9f,0xef,0xf3,0x7d,0xc4,0x8a,0x97,0x97,0xf9,0x3e,0x3f,0x2f,0x1f,0x31,0xdf,
        0xe7,0xfb,0x7c,0x3f,0xa9,0x9e,0xe7,0xeb,0xca,0xf3,0xfd,0x65,0xbe,0xbf,0x9c,0xed,
        0x2f,0xf3,0x7d,0xfe,0xf6,0xf9,0xf2,0xb3,0x7b,0x5f,0xcf,0x00,0x80,0xa9,0xba,0xef,
        
        0x7d,0xfe,0xed,0x33,0x5e,0xcc,0x38,0xcf,0xf7,0xf9,0x79,0xd0,0x89,0x7b,0xc5,0x7d,
        0x43,0xd0,0x5f,0xe6,0xfb,0x7c,0xbf,0xef,0x55,0xcf,0xfb,0x7c,0x9f,0xef,0x23,0x56,
        0xbc,0xbc,0xcc,0xf7,0xf9,0xf9,0x6a,0xf0,0x7c,0x9f,0xef,0xf3,0xfd,0xa4,0x7a,0x9e,
        0xaf,0x2b,0xcf,0x77,0x48,0x75,0xbf,0x7c,0xfb,0xbe,0xe2,0x9e,0xbf,0x7d,0xbe,0xfc,
        0xdb,0xcf,0xeb,0x19,0x00,0x80,0xa9,0xba,0xef,0x7d,0x7e,0x3e,0x83,0xe7,0xe5,0x37,
        0x78,0x9e,0xc3,0x1b,0x9e,0x07,0x9d,0x7f,0x83,0xbf,0x97,0x51,0xe1,0x0d,0xef,0xf9,
        0x7e,0xdf,0xab,0x9e,0x77,0x78,0xcf,0x6f,0x08,0x9f,0x0c,0xe9,0x17,0x70,0x9f,0x9f,
        0x5f,0x56,0xc0,0x7b,0xbe,0x83,0xfb,0x49,0xf5,0x3c,0x5f,0x57,0x85,0x13,0x13,0x1f,
        0xbc,0xef,0x7b,0x87,0x13,0x67,0xc8,0x6e,0x38,0xe2,0xbe,0xcf,0xeb,0x19,0x00,0x80,
        0xa9,0x9a,0xef,0xe0,0x79,0xe0,0x11,0x2b,0x46,0xbc,0x9c,0x07,0x3d,0x9f,0xcf,0xce,
        0xe7,0xab,0x91,0x5f,0xee,0xf3,0x7c,0x9f,0xaf,0x7a,0xde,0xef,0xf3,0xfc,0xe5,0x3c,
        0xea,0xfe,0x32,0xdf,0xe7,0xe7,0x89,0xc9,0x2f,0xf3,0x7d,0xbe,0x9f,0x54,0xcf,0xf3,
        0x75,0xe6,0x97,0xf3,0x79,0xbe,0xef,0xdf,0x3e,0x3f,0x9f,0xe7,0x6f,0x1f,0x78,0xff,
        0xce,0xeb,0x19,0x00,0x80,0xa9,0x9a,0xef,0xf3,0xf3,0xc0,0x23,0x56,0x8c,0x78,0x39,
        0x0f,0x7a,0x06,0x3f,0x67,0xf0,0x65,0xe4,0x97,0xef,0x9b,0xef,0xf3,0x55,0xcf,0xfb,
        0xf7,0xcd,0x5f,0xce,0xa3,0xee,0x2f,0xf3,0x7d,0x7e,0x9e,0xcf,0x78,0x99,0xef,0xf3,
        
        0xfd,0xa4,0x7a,0x9e,0xaf,0x33,0xbf,0x9c,0xcf,0xf3,0xdd,0x79,0x9f,0x9f,0xcf,0xf3,
        0xb7,0x0f,0xec,0x7c,0x5e,0xcf,0x00,0x80,0xa9,0x9a,0xef,0xf3,0xf3,0xc0,0xe7,0xf9,
        0x8a,0xe5,0x2f,0xf3,0x41,0x97,0x0f,0x7c,0x9f,0x47,0x7d,0x39,0xdf,0xf3,0x7d,0xbe,
        0xea,0x77,0x9f,0xef,0xf9,0x3e,0x3f,0xaf,0x98,0xdf,0xe7,0x7d,0xbe,0xcf,0xcf,0xe7,
        0x81,0x5f,0xe6,0xfb,0x7c,0x3f,0xa9,0x9e,0xe7,0xeb,0xd2,0xf7,0x79,0x7e,0x9e,0xef,
        0xf3,0x7d,0x7e,0x3e,0xcf,0xdf,0xbe,0xdc,0x79,0x7e,0x5e,0xcf,0x00,0x80,0xa9,0x9a,
        0xef,0xf3,0xf3,0x64,0xc8,0x41,0xc0,0x90,0x43,0x33,0x9a,0x21,0xe8,0x90,0x65,0xe0,
        0x0f,0x45,0x7f,0x99,0xdf,0x90,0x9e,0xaf,0x0c,0xb9,0xf9,0x0d,0x39,0x08,0x9f,0x0c,
        0xc9,0x68,0xcf,0x6f,0x08,0x3f,0x83,0xcb,0xaf,0x20,0x3d,0xdf,0xc1,0x13,0xe5,0x1c,
        0xa2,0x93,0xa2,0x19,0x25,0xe6,0x90,0x83,0x1c,0x84,0xcf,0x21,0xbb,0x97,0x9b,0x43,
        0x0e,0xa2,0x37,0x00,0x80,0xb2,0x59,0x01,0x00,0x80,0xb2,0x59,0x01,0x00,0x80,0xb2,
        0x59,0x01,0x00,0x80,0xb2,0x59,0x01,0x00,0x80,0xb2,0x59,0x01,0x00,0x80,0x5b,0xca,
        0x5c,0x38,0x14,0x81,0x62,0x85,0x83,0x15,0x67,0x97,0x04,0xe6,0x8a,0x9e,0x72,0x2a,
        0xb0,0xd0,0x51,0x61,0xd3,0xb0,0x09,0xe4,0x11,0x47,0x02,0x1e,0x10,0x1c,0x92,0x41,
        0x7f,0x98,0xc5,0xa0,0x1c,0x1d,0x34,0x83,0x67,0x7c,0x78,0xd0,0x01,0xfb,0xa0,0x54,
        0x38,0x20,0x8d,0x07,0x19,0x34,0x83,0x64,0xf0,0x05,0x22,0xd0,0xc3,0x07,0x9b,0x41,
        
        0x06,0x1d,0x94,0x80,0x79,0x05,0xf3,0x03,0x1d,0x06,0x42,0x80,0x9e,0x91,0x41,0x07,
        0x74,0xfb,0x01,0x00,0x80,0xa9,0xc0,0x40,0x84,0x06,0x4b,0x9c,0xb1,0x90,0x26,0xb0,
        0x70,0x15,0x96,0x82,0x85,0x8e,0x0e,0x8b,0x86,0xf9,0x41,0x06,0xe2,0x48,0xc0,0x03,
        0x16,0x19,0x3b,0xfa,0x30,0x8b,0x41,0x39,0xe2,0x48,0x86,0x91,0xd1,0x41,0x1b,0x3f,
        0x6c,0x06,0xc5,0xe0,0x0a,0xe3,0x70,0xc7,0x87,0x34,0x92,0xc1,0x0b,0x2a,0x47,0x61,
        0xf4,0xf0,0xa1,0x83,0x34,0x7a,0xe8,0x20,0x02,0x16,0x0b,0x6c,0x06,0x74,0x18,0xe6,
        0x19,0xe6,0x0f,0x69,0x74,0x40,0xb7,0x1f,0x00,0x80,0x69,0x01,0xa1,0xa8,0x82,0xba,
        0x1c,0xe6,0x48,0xfc,0x38,0x07,0x66,0x6e,0x52,0x58,0xe8,0x54,0x40,0x3c,0x18,0x0f,
        0x39,0xc4,0x91,0x80,0x07,0x10,0x05,0x0d,0xfc,0x30,0x2b,0x47,0x38,0x10,0x05,0x3b,
        0xfa,0xe8,0xa0,0x8d,0x1e,0x6c,0x1a,0x3d,0x74,0x90,0x15,0x2c,0x06,0xec,0x0f,0xa3,
        0xa0,0x72,0x88,0x40,0x02,0x19,0xf4,0xd1,0x46,0x06,0x1d,0x94,0x40,0x1b,0x59,0xd0,
        0x46,0x07,0x41,0xc0,0x66,0x60,0x73,0x50,0x47,0x06,0x74,0xfb,0x01,0x00,0x80,0x09,
        0x86,0x81,0x8a,0xae,0xd0,0x07,0x1d,0xeb,0x85,0xb0,0x29,0x5b,0x15,0x16,0x3a,0x0d,
        0x52,0xc1,0x7c,0x20,0x83,0x34,0x32,0x68,0x03,0x51,0xd0,0x40,0x07,0x75,0xa8,0xa3,
        0x72,0x94,0x83,0x2e,0xa8,0xf1,0xe3,0x43,0x1b,0x1d,0xb0,0x0f,0x1a,0xd0,0x41,0x57,
        0x98,0x3d,0xb0,0x3f,0xe8,0x19,0xc5,0xc0,0x33,0x6c,0x06,0x7d,0x74,0xd0,0xc3,0x87,
        
        0x09,0x74,0xd0,0x05,0x1d,0x3c,0x28,0x1d,0x6d,0x64,0x90,0x46,0x06,0xee,0xe8,0x20,
        0x76,0x18,0x00,0x80,0x69,0x1d,0x07,0xc1,0x02,0x59,0x20,0x05,0x06,0xc3,0x2b,0x21,
        0x68,0xf0,0xa7,0x0c,0x0b,0x1d,0x04,0xa9,0x10,0x1c,0x39,0xa4,0x21,0x0d,0x09,0xa4,
        0x61,0xfe,0xb0,0x18,0xd4,0xa1,0x8e,0x31,0x28,0x07,0x5d,0xe0,0xc0,0x8f,0x07,0x69,
        0xf4,0x60,0x33,0x68,0x43,0xde,0x72,0x10,0x1c,0xe4,0xc1,0x7e,0x38,0x70,0x46,0x11,
        0x20,0x07,0x9b,0x41,0x1f,0x09,0x64,0xd0,0x41,0x09,0x7c,0xe8,0x02,0xc4,0x83,0x2f,
        0xc8,0x20,0x81,0x1c,0x12,0xa0,0x18,0xd4,0xbe,0x00,0x80,0xa9,0x02,0x43,0x02,0xfa,
        0x22,0x1c,0xe1,0x20,0x8c,0x6a,0xe1,0xd5,0x90,0x81,0x01,0x0d,0x0b,0x1d,0x0a,0xa9,
        0x10,0x47,0x0e,0x36,0x03,0xf6,0x46,0x1c,0x39,0xd8,0x34,0x1a,0x48,0xa0,0x72,0x94,
        0x03,0x91,0x61,0x7e,0x68,0xa3,0x83,0x1e,0x6c,0x06,0x1d,0x04,0x08,0xda,0xf0,0x07,
        0xfb,0xe1,0x40,0x1f,0x2b,0xa0,0x03,0x9b,0x41,0x1f,0x6d,0x64,0xd0,0x41,0x09,0x74,
        0x10,0x18,0x8c,0x0c,0x63,0x41,0x0f,0x0d,0xe4,0xd0,0x01,0xfb,0x80,0xdb,0x33,0x00,
        0x80,0x09,0x03,0x3a,0x04,0x16,0x30,0x08,0x47,0x81,0x19,0xd9,0xcd,0x19,0x02,0x0d,
        0x0b,0x5d,0x09,0xad,0x91,0x40,0x0e,0xe6,0x0f,0x8b,0x41,0x02,0x69,0xb0,0x37,0x6e,
        0xa4,0xb1,0x1c,0xe5,0xa8,0xa3,0x0f,0x8b,0x83,0x45,0x23,0x83,0x06,0x74,0x90,0xc1,
        0x58,0x90,0x86,0x66,0x74,0xd0,0x43,0x71,0x24,0xa3,0x64,0xa4,0x21,0x38,0x64,0xd0,
        
        0x47,0x1b,0x69,0x64,0x50,0x02,0x1d,0x74,0x01,0xe2,0x61,0x2c,0xe8,0x20,0x81,0x66,
        0xb4,0xc1,0x3e,0x30,0xec,0x0f,0x00,0x80,0x89,0x0e,0x03,0x0c,0x0a,0x62,0x5c,0x38,
        0xe6,0xc2,0x43,0xa6,0x4f,0x8e,0xcf,0x84,0x94,0x61,0xae,0xab,0x22,0x2b,0x24,0x20,
        0x07,0x06,0x07,0xe3,0x90,0x46,0x1c,0x32,0x88,0x23,0x8d,0x1b,0xad,0x91,0x82,0x36,
        0x7a,0xb0,0x69,0x58,0x1c,0x3a,0x68,0x43,0x0e,0x16,0x87,0xe5,0x90,0x81,0x64,0x74,
        0xd0,0x83,0xf9,0x21,0x12,0xb4,0xd1,0x41,0x0f,0x3f,0xda,0xc8,0xa0,0x83,0x0a,0xa4,
        0x11,0x18,0x09,0x64,0xc0,0x57,0x74,0x90,0x40,0x0e,0x69,0xd8,0x1c,0xb8,0x3d,0x03,
        0x00,0x80,0x09,0x82,0x60,0xc1,0x82,0x80,0x23,0xe1,0x07,0x03,0x83,0x83,0xa4,0x60,
        0x61,0xaf,0x69,0x10,0x8e,0x54,0x70,0x8e,0x36,0x2c,0x1e,0x75,0x74,0x70,0xa3,0x35,
        0x52,0x90,0x46,0x02,0x19,0xb4,0x61,0xfe,0xe8,0xa0,0x0d,0x79,0x20,0x0e,0xa9,0x91,
        0x83,0x64,0x74,0x90,0x86,0xcd,0x20,0xf2,0x07,0x1a,0xe8,0xa0,0x87,0x36,0x32,0x90,
        0x40,0x06,0x11,0x68,0x23,0x30,0xd2,0x48,0x23,0x08,0x7c,0x68,0x23,0x83,0x34,0x28,
        0x06,0xc4,0x5e,0x00,0x80,0x49,0x4d,0x58,0xc2,0x61,0x43,0x80,0x00,0x41,0x1f,0x8a,
        0x0b,0x0c,0x0b,0x15,0x58,0xb0,0x53,0x87,0x0b,0x81,0x05,0x3a,0x40,0x64,0xdc,0xa0,
        0x73,0x7c,0xe8,0x20,0x19,0xf4,0x47,0x6a,0xe4,0x90,0x82,0x0e,0x12,0x60,0x1f,0xa4,
        0x06,0xa2,0xd1,0x41,0x1f,0x16,0x07,0x9b,0x01,0x1f,0xd8,0x2c,0x48,0x23,0x8d,0xd2,
        
        0x91,0x40,0x06,0x1d,0x64,0x60,0x73,0x28,0xe6,0xee,0xfc,0xe3,0xc2,0x3d,0x48,0xe1,
        0x77,0x52,0x02,0x00,0x80,0x49,0x22,0xf0,0x02,0x18,0x1c,0x04,0x18,0x5a,0x5c,0xf3,
        0xc3,0x1c,0x1a,0x16,0xa6,0x68,0x2e,0xfc,0x58,0xc0,0x39,0x21,0xd0,0x47,0x0b,0x3a,
        0x88,0x43,0x06,0x69,0xa4,0x06,0x63,0xd0,0x41,0x1f,0xe6,0x0f,0x8b,0x83,0xd1,0xb0,
        0x58,0xc1,0x1e,0xa0,0xc3,0xb0,0x39,0x24,0x90,0x41,0x1c,0x3e,0x28,0x12,0x83,0xb5,
        0x96,0x0b,0x25,0x68,0x00,0x80,0x09,0x0a,0x2d,0x30,0x5d,0xe0,0xc0,0xb9,0x70,0x2c,
        0xe1,0x47,0x4f,0xca,0x30,0x37,0xc7,0x18,0xe6,0xa7,0xa3,0x8c,0x0f,0x63,0xc0,0x1f,
        0x1d,0xb4,0xc0,0x10,0x60,0x3c,0x8c,0x86,0x79,0x05,0x9b,0x01,0x9d,0x80,0x45,0x86,
        0xc5,0xa0,0x0d,0x1d,0x70,0xc2,0xde,0x1c,0x8f,0x16,0xc3,0x3c,0x94,0x94,0x01,0x00,
        0x80,0x09,0x04,0xe4,0x85,0x23,0x0e,0xa3,0x60,0x5c,0x48,0x0f,0x2a,0xf1,0x63,0x12,
        0x58,0x36,0x43,0x2d,0x79,0x99,0x61,0xa4,0x86,0x72,0xf0,0x01,0x1d,0x03,0xf3,0x07,
        0x9d,0x80,0x39,0x07,0xe2,0xd0,0x41,0x06,0x0c,0xce,0x05,0x8d,0xdf,0x81,0xd7,0x70,
        0x48,0x82,0x02,0x00,0x80,0x89,0x1a,0x16,0x0d,0x19,0xc2,0x71,0x11,0x2e,0x45,0xdc,
        0x01,0x59,0x2a,0xf4,0xa4,0x04,0x13,0xb0,0x16,0xc2,0x65,0x46,0x9b,0x71,0x4c,0x80,
        0xc3,0xa0,0xaf,0xe0,0x19,0x1d,0x64,0x40,0x17,0x22,0x9e,0x42,0xb9,0x18,0xcc,0xa7,
        0x04,0x05,0x00,0x80,0x89,0x0a,0xe8,0x38,0xfc,0x40,0x80,0x91,0x82,0x82,0xe9,0x85,
};

static  unsigned char   g3white[] =
{
    /*code  codelen=4 */
    /*00*/    0,  0,  0,  0,  0,  0,  0,  3,  4,  0,  0,  5,  6,  0,  7,  8,
    /*10*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*30*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*code  codelen=5 */
    /*00*/    0,  0,  0,  0,  0,  0,  0, 11, 12,  0,  0,  0,  0,  0,  0,  0,
    /*10*/    0,  0, 67,  9, 10,  0,  0,  0,  0,  0,  0, 66,  0,  0,  0,  0,
    /*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*30*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*code  codelen=6 */
    /*00*/    0,  0,  0, 14,  0,  0,  0,  2, 13,  0,  0,  0,  0,  0,  0,  0,
    /*10*/    0,  0,  0,  0,  0,  0,  0, 68, 91,  0,  0,  0,  0,  0,  0,  0,
    /*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 17, 18,  0,  0,  0,  0,
    /*30*/    0,  0,  0,  0, 15, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*code  codelen=7 */
    /*00*/    0,  0,  0, 23, 24,  0,  0,  0, 21,  0,  0,  0, 20,  0,  0,  0,
    /*10*/    0,  0,  0, 27,  0,  0,  0, 22, 29,  0,  0,  0,  0,  0,  0,  0,
    /*20*/    0,  0,  0,  0, 28,  0,  0, 19, 25,  0,  0, 26,  0,  0,  0,  0,
    /*30*/    0,  0,  0,  0,  0,  0,  0, 69,  0,  0,  0,  0,  0,  0,  0,  0,
    /*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*code  codelen=8 */
    /*00*/    0,  0, 30, 31, 46, 47,  0,  0,  0,  0, 48, 49,  0,  0,  0,  0,
    /*10*/    0,  0, 34, 35, 36, 37, 38, 39,  0,  0, 32, 33,  0,  0,  0,  0,
    /*20*/    0,  0,  0,  0, 54, 55,  0,  0, 40, 41, 42, 43, 44, 45,  0,  0,
    /*30*/    0,  0, 62, 63, 64,  1, 70, 71,  0,  0,  0,  0,  0,  0,  0,  0,
    /*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 60, 61,  0,  0,  0,  0,
    /*50*/    0,  0, 50, 51, 52, 53,  0,  0, 56, 57, 58, 59,  0,  0,  0,  0,
    /*60*/    0,  0,  0,  0, 72, 73,  0, 75, 74,  0,  0,  0,  0,  0,  0,  0,
    /*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*code  codelen=9 */
    /*00*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*10*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*30*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*90*/    0,  0,  0,  0,  0,  0,  0,  0, 88, 89, 90, 92,  0,  0,  0,  0,
    /*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 76, 77,  0,  0,
    /*d0*/    0,  0, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87,  0,  0,  0,  0,
/*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*code  codelen=10 */
/*00*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*10*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*30*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*code  codelen=11 */
/*00*/    0,255,  0,  0,  0,  0,  0,  0, 93,  0,  0,  0, 94, 95,  0,  0,
/*10*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*30*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*code  codelen=12 */
/*00*/    0,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*10*/    0,  0, 96, 97, 98, 99,100,101,  0,  0,  0,  0,102,103,104,105,
/*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*30*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

static  unsigned char   g3black[] =
{
    /*code  codelen=2 */
    /*00*/    0,  0,  4,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*10*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*30*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*code  codelen=3 */
    /*00*/    0,  0,  2,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*10*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*30*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*code  codelen=4 */
    /*00*/    0,  0,  7,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*10*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*30*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*code  codelen=5 */
    /*00*/    0,  0,  0,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*10*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*30*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*code  codelen=6 */
    /*00*/    0,  0,  0,  0, 10,  9,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*10*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*30*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*code  codelen=7 */
    /*00*/    0,  0,  0,  0, 11, 12,  0, 13,  0,  0,  0,  0,  0,  0,  0,  0,
    /*10*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*30*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    /*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*code  codelen=8 */
/*00*/    0,  0,  0,  0, 14,  0,  0, 15,  0,  0,  0,  0,  0,  0,  0,  0,
/*10*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*30*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*code  codelen=9 */
/*00*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*10*/    0,  0,  0,  0,  0,  0,  0,  0, 16,  0,  0,  0,  0,  0,  0,  0,
/*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*30*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*code  codelen=10 */
/*00*/    0,  0,  0,  0,  0,  0,  0,  0, 19,  0,  0,  0,  0,  0,  0, 66,
/*10*/    0,  0,  0,  0,  0,  0,  0, 17, 18,  0,  0,  0,  0,  0,  0,  0,
/*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*30*/    0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,
/*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*60*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*code  codelen=11 */
/*00*/    0,255,  0,  0,  0,  0,  0,  0, 93,  0,  0,  0, 94, 95,  0,  0,
/*10*/    0,  0,  0,  0,  0,  0,  0, 25, 26,  0,  0,  0,  0,  0,  0,  0,
/*20*/    0,  0,  0,  0,  0,  0,  0,  0, 24,  0,  0,  0,  0,  0,  0,  0,
/*30*/    0,  0,  0,  0,  0,  0,  0, 23,  0,  0,  0,  0,  0,  0,  0,  0,
/*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*50*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*60*/    0,  0,  0,  0,  0,  0,  0, 20, 21,  0,  0,  0, 22,  0,  0,  0,
/*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*code  codelen=12 */
/*00*/    0,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*10*/    0,  0, 96, 97, 98, 99,100,101,  0,  0,  0,  0,102,103,104,105,
/*20*/    0,  0,  0,  0, 53,  0,  0, 56, 57,  0,  0, 60, 61,  0,  0,  0,
/*30*/    0,  0,  0, 70, 71, 72,  0, 54, 55,  0,  0,  0,  0,  0,  0,  0,
/*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*50*/    0,  0, 51, 52, 45, 46, 47, 48, 58, 59, 62, 69,  0,  0,  0,  0,
/*60*/    0,  0,  0,  0, 49, 50, 63, 64, 31, 32, 33, 34, 41, 42,  0,  0,
/*70*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*c0*/    0,  0,  0,  0,  0,  0,  0,  0, 67, 68, 27, 28, 29, 30,  0,  0,
/*d0*/    0,  0, 35, 36, 37, 38, 39, 40,  0,  0, 43, 44,  0,  0,  0,  0,
/*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*code  codelen=13 */
/*00*/    0,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*10*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*20*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*30*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*40*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 75, 76, 77, 78,  0,  0,
/*50*/    0,  0, 85, 86, 87, 88,  0,  0,  0,  0, 89, 90,  0,  0,  0,  0,
/*60*/    0,  0,  0,  0, 91, 92,  0,  0,  0,  0,  0,  0, 73, 74,  0,  0,
/*70*/    0,  0, 79, 80, 81, 82, 83, 84,  0,  0,  0,  0,  0,  0,  0,  0,
/*80*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*90*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*a0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*b0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*c0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*d0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*e0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*f0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

static  unsigned int    lastbyte = 0;
static  int     bitsleft = 0;
static  unsigned char   *nextbyte = fax;
void    rowout(register unsigned char *rowbuf, register int maxcol, 
               register int currow);

int getbit()
{
    int     bbit;
    
    if (--bitsleft < 0)
    {   if (nextbyte >= &fax[sizeof(fax)])
    {   lastbyte = 0;
    eof = 1;
    }
    else
    {   lastbyte = *nextbyte++;
    bitsleft = 7;
    }
    }
    bbit = lastbyte & 0x1;
    lastbyte >>= 1;
    return (bbit);
}

void    skiptoeol()
{
    int n;
    
    for (n = 0 ; n < 11 ; )
        if (getbit() == 0)
            n++;
        while (getbit() == 0)
            ;
}

void    rowout(rowbuf, maxcol, currow)
register unsigned char  *rowbuf;
register int        maxcol;
register int        currow;
{
    unsigned char   *s;
    int     i;
    unsigned long   sm;
    
    for (i = 0, s = rowbuf, sm = sum ; i < maxcol ; i++, s++)
        sm += *s;
    sum = sm;
}

#define G3_WHITE    0
#define G3_BLACK    1

int main(argc, argv)
int argc;
unsigned char   *argv[];
{
    unsigned char   code;
    unsigned char   *runtab, *runetab;
    int     runcode, runlen, runcolor;
    int     curcol, currow;
    
    eof = 0;
    sum = 0;
    eol = 0;
    lastbyte = 0;
    bitsleft = 0;
    nextbyte = fax;
    skiptoeol();
    for (currow = 0 ; !eof ; currow++)
    {    
        runlen = 0;
        code = getbit();
        code = (code<<1) + getbit();
        code = (code<<1) + getbit();
        runcolor = G3_WHITE;
        runtab = g3white;
        runetab = &g3white[sizeof(g3white)];
        
        for (curcol = 0 ; !eof ; )
        {    
            code = (code<<1) + getbit();
            runcode = runtab[code];
            if (runcode == 0)
            {    
                runtab += 256;
                if (runtab < runetab)
                    continue;
                if (code != 1)
                {    
                    puts("corrupted fax");
                    skiptoeol();
                    break;
                }
                runcode = 255;   /* indicate eol code */
            }
            runcode--;
            if (runcode >= 64)
            {    if (runcode >= 254)
            {    
                if (curcol != 0)
                    eol = 0;
                else if (++eol >= 3)
                    eof = 1;
                break;
            }
            runlen += (runcode-64) * 64;
            }
            else
            {    
                runlen += runcode;
                if (curcol + runlen > MAXCOLS)
                    runlen = MAXCOLS - curcol;
                for ( ; --runlen >= 0 ; curcol++)
                    rowbuf[curcol] = runcolor;
                runlen = 0;
                if (curcol >= MAXCOLS)
                {    
                    skiptoeol();
                    break;
                }
                runcolor = (runcolor == G3_WHITE)
                    ? G3_BLACK : G3_WHITE;
            }
            if (runcolor == G3_BLACK)
            {    
                runtab = g3black;
                runetab = &g3black[sizeof(g3black)];
                code = getbit();
            }
            else
            {    
                runtab = g3white;
                runetab = &g3white[sizeof(g3white)];
                code = getbit();
                code = (code<<1) + getbit();
                code = (code<<1) + getbit();
            }
        }
        rowout(rowbuf, MAXCOLS, currow);
    }
    
    if (sum != faxsum) {    
        puts("g3fax: fail\n");
    }
    else {
        puts("g3fax: success\n");
    }
    return 0;
}

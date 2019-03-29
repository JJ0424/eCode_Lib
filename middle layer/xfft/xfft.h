
/*----------------------------------------------------------------------^^-
/ File name:  xfft.h
/ Author:     JiangJun
/ Data:       [2019-3-27]
/ Version:    v1.1
/-----------------------------------------------------------------------^^-
/ Common FFT driver (Based on 'fix_fft' lib)
/------------------------------------------------------------------------*/

#ifndef _XFFT_H
#define _XFFT_H


//------------------------------------------------------------
// FFT Config
// ---
// Runtime: (IAR 6.3 Medime Optimize/ STM32F103(72MHz))
// 16: 79us         32: 184us           64: 421us
// 128: 948us       256: 2.112ms        512: 4.660ms
// 1024: 10.19ms
//------------------------------------------------------------

// Number List
#define _XFFT_NUM_16            16
#define _XFFT_NUM_32            32
#define _XFFT_NUM_64            64
#define _XFFT_NUM_128           128
#define _XFFT_NUM_256           256
#define _XFFT_NUM_512           512
#define _XFFT_NUM_1024          1024

#define _XFFT_NUM_FFT           _XFFT_NUM_128   

// Sample Rate
#define _XFFT_SAMPLE_RATE       50000       // 50KHz


//------------------------------------------------------------
// FFT Type
//------------------------------------------------------------

typedef unsigned char       fft_u8;
typedef unsigned short int  fft_u16;
typedef unsigned int        fft_u32;

typedef signed char         fft_s8;
typedef signed short int    fft_s16;
typedef signed int          fft_s32;


// FFT data source
typedef struct {

    fft_s16 real[_XFFT_NUM_FFT];    // -32768-32767
    fft_s16 imag[_XFFT_NUM_FFT];    // -32768-32767
    
} XFFT_SrcT;

// FFT Power data 
typedef struct {

    // Output Table Size
    fft_u16 fft_list_size;   

    // Frequency Resolution: Sample Rate / _XFFT_NUM_FFT(0 - 65535)
    fft_u16 fft_freq_ratio;   

    // Frequency Table
#if (_XFFT_SAMPLE_RATE > 65535)

    fft_u32 fft_sample_rate;                        // Sample Rate
    fft_u32 fft_freq_table[_XFFT_NUM_FFT >> 1];     // 0 - ...Hz
#else

    fft_u16 fft_sample_rate;                        // Sample Rate
    fft_u16 fft_freq_table[_XFFT_NUM_FFT >> 1];     // 0 - 65535Hz
#endif

    // Complex Power
    fft_s16 fft_cmpx_pwr[_XFFT_NUM_FFT >> 1];       // 0 - 32767
    
} XFFT_CmplxMagT;


// FFT Band Filter Output
typedef struct {

    fft_u32 band_low;           // user Input Band Low Frequency
    fft_u32 band_high;          // user Input Band High Frequency    

    // the max/min Power of the specified Band Filter
    fft_s16 cmpx_pwr_max, cmpx_pwr_min;

    // the max/min Frequency of the specified Band Filter
    fft_u32 cmpx_freq_max, cmpx_freq_min;
    
} XFFT_BandFilterT;


extern void xfft(XFFT_SrcT *fft);
extern void xfft_cmplx_mag(XFFT_SrcT *fft, XFFT_CmplxMagT *cmplx_mag);
extern void xfft_band_filter(XFFT_CmplxMagT *cmplx_mag, fft_u32 band_low, fft_u32 band_high, XFFT_BandFilterT *filter_out);

#endif
//---------------------------------------------------------------------------//
//----------------------------- END OF FILE ---------------------------------//
//---------------------------------------------------------------------------//

#ifndef STK_STK_H
#define STK_STK_H

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
//#include <cstdlib>
//#include "alax-math/alax-math.h"
#include <cmath>

/*! \namespace stk
    \brief The STK namespace.

    Most Stk classes are defined within the STK namespace.  Exceptions
    to this include the classes RtAudio and RtMidi.
*/




namespace stk {

/***************************************************/
/*! \class Stk
    \brief STK base class

    Nearly all STK classes inherit from this class.
    The global sample rate and rawwave path variables
    can be queried and modified via Stk.  In addition,
    this class provides error handling and
    byte-swapping functions.

    The Synthesis ToolKit in C++ (STK) is a set of open source audio
    signal processing and algorithmic synthesis classes written in the
    C++ programming language. STK was designed to facilitate rapid
    development of music synthesis and audio processing software, with
    an emphasis on cross-platform functionality, realtime control,
    ease of use, and educational example code.  STK currently runs
    with realtime support (audio and MIDI) on Linux, Macintosh OS X,
    and Windows computer platforms. Generic, non-realtime support has
    been tested under NeXTStep, Sun, and other platforms and should
    work with any standard C++ compiler.

    STK WWW site: http://ccrma.stanford.edu/software/stk/

    The Synthesis ToolKit in C++ (STK)
    Copyright (c) 1995--2016 Perry R. Cook and Gary P. Scavone

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation files
    (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    Any person wishing to distribute modifications to the Software is
    asked to send the modifications to the original developer so that
    they can be incorporated into the canonical version.  This is,
    however, not a binding provision of this license.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/***************************************************/

//#define _STK_DEBUG_

// Most data in STK is passed and calculated with the
// following user-definable floating-point type.  You
// can change this to "float" if you prefer or perhaps
// a "long double" in the future.
typedef float StkFloat;

//! STK error handling class.
/*!
  This is a fairly abstract exception handling class.  There could
  be sub-classes to take care of more specific error conditions ... or
  not.
*/
class StkError
{
public:
  enum Type {
    STATUS,
    WARNING,
    DEBUG_PRINT,
    MEMORY_ALLOCATION,
    MEMORY_ACCESS,
    FUNCTION_ARGUMENT,
    FILE_NOT_FOUND,
    FILE_UNKNOWN_FORMAT,
    FILE_ERROR,
    PROCESS_THREAD,
    PROCESS_SOCKET,
    PROCESS_SOCKET_IPADDR,
    AUDIO_SYSTEM,
    MIDI_SYSTEM,
    UNSPECIFIED
  };

protected:
  std::string message_;
  Type type_;

public:
  //! The constructor.
  StkError(const std::string& message, Type type = StkError::UNSPECIFIED)
    : message_(message), type_(type) {}

  //! The destructor.
  virtual ~StkError(void) {};

  //! Prints thrown error message to stderr.
  virtual void printMessage(void) { std::cerr << '\n' << message_ << "\n\n"; }

  //! Returns the thrown error message type.
  virtual const Type& getType(void) { return type_; }

  //! Returns the thrown error message string.
  virtual const std::string& getMessage(void) { return message_; }

  //! Returns the thrown error message as a C string.
  virtual const char *getMessageCString(void) { return message_.c_str(); }
};


class Stk
{
public:

  typedef unsigned long StkFormat;
  static const StkFormat STK_SINT8;   /*!< -128 to +127 */
  static const StkFormat STK_SINT16;  /*!< -32768 to +32767 */
  static const StkFormat STK_SINT24;  /*!< Lower 3 bytes of 32-bit signed integer. */
  static const StkFormat STK_SINT32;  /*!< -2147483648 to +2147483647. */
  static const StkFormat STK_FLOAT32; /*!< Normalized between plus/minus 1.0. */
  static const StkFormat STK_FLOAT64; /*!< Normalized between plus/minus 1.0. */

  //! Static method that returns the current STK sample rate.
  static StkFloat sampleRate( void ) { return srate_; }

  //! Static method that sets the STK sample rate.
  /*!
    The sample rate set using this method is queried by all STK
    classes that depend on its value.  It is initialized to the
    default SRATE set in Stk.h.  Many STK classes use the sample rate
    during instantiation.  Therefore, if you wish to use a rate that
    is different from the default rate, it is imperative that it be
    set \e BEFORE STK objects are instantiated.  A few classes that
    make use of the global STK sample rate are automatically notified
    when the rate changes so that internal class data can be
    appropriately updated.  However, this has not been fully
    implemented.  Specifically, classes that appropriately update
    their own data when either a setFrequency() or noteOn() function
    is called do not currently receive the automatic notification of
    rate change.  If the user wants a specific class instance to
    ignore such notifications, perhaps in a multi-rate context, the
    function Stk::ignoreSampleRateChange() should be called.
  */
  static void setSampleRate( StkFloat rate );

  //! A function to enable/disable the automatic updating of class data when the STK sample rate changes.
  /*!
    This function allows the user to enable or disable class data
    updates in response to global sample rate changes on a class by
    class basis.
  */
  void ignoreSampleRateChange( bool ignore = true ) { ignoreSampleRateChange_ = ignore; };
  
  //! Static method that frees memory from alertList_.
  static void  clear_alertList(){std::vector<Stk *>().swap(alertList_);};
  
  //! Static method that returns the current rawwave path.
  static std::string rawwavePath(void) { return rawwavepath_; }

  //! Static method that sets the STK rawwave path.
  static void setRawwavePath( std::string path );

  //! Static method that byte-swaps a 16-bit data type.
  static void swap16( unsigned char *ptr );

  //! Static method that byte-swaps a 32-bit data type.
  static void swap32( unsigned char *ptr );

  //! Static method that byte-swaps a 64-bit data type.
  static void swap64( unsigned char *ptr );

  //! Static cross-platform method to sleep for a number of milliseconds.
  static void sleep( unsigned long milliseconds );

  //! Static method to check whether a value is within a specified range.
  static bool inRange( StkFloat value, StkFloat min, StkFloat max ) {
    if ( value < min ) return false;
    else if ( value > max ) return false;
    else return true;
  }

  //! Static function for error reporting and handling using c-strings.
  static void handleError( const char *message, StkError::Type type );

  //! Static function for error reporting and handling using c++ strings.
  static void handleError( std::string message, StkError::Type type );

  //! Toggle display of WARNING and STATUS messages.
  static void showWarnings( bool status ) { showWarnings_ = status; }

  //! Toggle display of error messages before throwing exceptions.
  static void printErrors( bool status ) { printErrors_ = status; }

private:
  static StkFloat srate_;
  static std::string rawwavepath_;
  static bool showWarnings_;
  static bool printErrors_;
  static std::vector<Stk *> alertList_;

protected:

  static std::ostringstream oStream_;
  bool ignoreSampleRateChange_;

  //! Default constructor.
  Stk( void );

  //! Class destructor.
  virtual ~Stk( void );

  //! This function should be implemented in subclasses that depend on the sample rate.
  virtual void sampleRateChanged( StkFloat newRate, StkFloat oldRate );

  //! Add class pointer to list for sample rate change notification.
  void addSampleRateAlert( Stk *ptr );

  //! Remove class pointer from list for sample rate change notification.
  void removeSampleRateAlert( Stk *ptr );

  //! Internal function for error reporting that assumes message in \c oStream_ variable.
  void handleError( StkError::Type type ) const;
};


/***************************************************/
/*! \class StkFrames
    \brief An STK class to handle vectorized audio data.

    This class can hold single- or multi-channel audio data.  The data
    type is always StkFloat and the channel format is always
    interleaved.  In an effort to maintain efficiency, no
    out-of-bounds checks are performed in this class unless
    _STK_DEBUG_ is defined.

    Internally, the data is stored in a one-dimensional C array.  An
    indexing operator is available to set and retrieve data values.
    Alternately, one can use pointers to access the data, using the
    index operator to get an address for a particular location in the
    data:

      StkFloat* ptr = &myStkFrames[0];

    Note that this class can also be used as a table with interpolating
    lookup.

    Possible future improvements in this class could include functions
    to convert to and return other data types.

    by Perry R. Cook and Gary P. Scavone, 1995--2016.
*/
/***************************************************/

class StkFrames
{
public:

  //! The default constructor initializes the frame data structure to size zero.
  StkFrames( unsigned int nFrames = 0, unsigned int nChannels = 0 );

  //! Overloaded constructor that initializes the frame data to the specified size with \c value.
  StkFrames( const StkFloat& value, unsigned int nFrames, unsigned int nChannels );

  //! The destructor.
  ~StkFrames();

  // A copy constructor.
  StkFrames( const StkFrames& f );

  // Assignment operator that returns a reference to self.
  StkFrames& operator= ( const StkFrames& f );

  //! Subscript operator that returns a reference to element \c n of self.
  /*!
    The result can be used as an lvalue. This reference is valid
    until the resize function is called or the array is destroyed. The
    index \c n must be between 0 and size less one.  No range checking
    is performed unless _STK_DEBUG_ is defined.
  */
  StkFloat& operator[] ( size_t n );

  //! Subscript operator that returns the value at element \c n of self.
  /*!
    The index \c n must be between 0 and size less one.  No range
    checking is performed unless _STK_DEBUG_ is defined.
  */
  StkFloat operator[] ( size_t n ) const;
    
  //! Sum operator
  /*!
    The dimensions of the argument are expected to be the same as
    self.  No range checking is performed unless _STK_DEBUG_ is
    defined.
  */
  StkFrames operator+(const StkFrames &frames) const;

  //! Assignment by sum operator into self.
  /*!
    The dimensions of the argument are expected to be the same as
    self.  No range checking is performed unless _STK_DEBUG_ is
    defined.
  */
  void operator+= ( StkFrames& f );

  //! Assignment by product operator into self.
  /*!
    The dimensions of the argument are expected to be the same as
    self.  No range checking is performed unless _STK_DEBUG_ is
    defined.
  */
  void operator*= ( StkFrames& f );

  //! Channel / frame subscript operator that returns a reference.
  /*!
    The result can be used as an lvalue. This reference is valid
    until the resize function is called or the array is destroyed. The
    \c frame index must be between 0 and frames() - 1.  The \c channel
    index must be between 0 and channels() - 1.  No range checking is
    performed unless _STK_DEBUG_ is defined.
  */
  StkFloat& operator() ( size_t frame, unsigned int channel );

  //! Channel / frame subscript operator that returns a value.
  /*!
    The \c frame index must be between 0 and frames() - 1.  The \c
    channel index must be between 0 and channels() - 1.  No range checking
    is performed unless _STK_DEBUG_ is defined.
  */
  StkFloat operator() ( size_t frame, unsigned int channel ) const;

  //! Return an interpolated value at the fractional frame index and channel.
  /*!
    This function performs linear interpolation.  The \c frame
    index must be between 0.0 and frames() - 1.  The \c channel index
    must be between 0 and channels() - 1.  No range checking is
    performed unless _STK_DEBUG_ is defined.
  */
  StkFloat interpolate( StkFloat frame, unsigned int channel = 0 ) const;

  //! Returns the total number of audio samples represented by the object.
  size_t size() const { return size_; }; 

  //! Returns \e true if the object size is zero and \e false otherwise.
  bool empty() const;

  //! Resize self to represent the specified number of channels and frames.
  /*!
    Changes the size of self based on the number of frames and
    channels.  No element assignment is performed.  No memory
    deallocation occurs if the new size is smaller than the previous
    size.  Further, no new memory is allocated when the new size is
    smaller or equal to a previously allocated size.
  */
  void resize( size_t nFrames, unsigned int nChannels = 1 );

  //! Resize self to represent the specified number of channels and frames and perform element initialization.
  /*!
    Changes the size of self based on the number of frames and
    channels, and assigns \c value to every element.  No memory
    deallocation occurs if the new size is smaller than the previous
    size.  Further, no new memory is allocated when the new size is
    smaller or equal to a previously allocated size.
  */
  void resize( size_t nFrames, unsigned int nChannels, StkFloat value );

  //! Retrieves a single channel
  /*!
    Copies the specified \c channel into \c destinationFrames's \c destinationChannel. \c destinationChannel must be between 0 and destination.channels() - 1 and
    \c channel must be between 0 and channels() - 1. destination.frames() must be >= frames().
    No range checking is performed unless _STK_DEBUG_ is defined.
  */
  StkFrames& getChannel(unsigned int channel,StkFrames& destinationFrames, unsigned int destinationChannel) const;

  //! Sets a single channel
  /*!
    Copies the \c sourceChannel of \c sourceFrames into the \c channel of self.
    SourceFrames.frames() must be equal to frames().
    No range checking is performed unless _STK_DEBUG_ is defined.
  */
  void setChannel(unsigned int channel,const StkFrames &sourceFrames,unsigned int sourceChannel);

  //! Return the number of channels represented by the data.
  unsigned int channels( void ) const { return nChannels_; };

  //! Return the number of sample frames represented by the data.
  unsigned int frames( void ) const { return (unsigned int)nFrames_; };

  //! Set the sample rate associated with the StkFrames data.
  /*!
    By default, this value is set equal to the current STK sample
    rate at the time of instantiation.
   */
  void setDataRate( StkFloat rate ) { dataRate_ = rate; };

  //! Return the sample rate associated with the StkFrames data.
  /*!
    By default, this value is set equal to the current STK sample
    rate at the time of instantiation.
   */
  StkFloat dataRate( void ) const { return dataRate_; };

private:

  StkFloat *data_;
  StkFloat dataRate_;
  size_t nFrames_;
  unsigned int nChannels_;
  size_t size_;
  size_t bufferSize_;

};

inline bool StkFrames :: empty() const
{
  if ( size_ > 0 ) return false;
  else return true;
}

inline StkFloat& StkFrames :: operator[] ( size_t n )
{
#if defined(_STK_DEBUG_)
  if ( n >= size_ ) {
    std::ostringstream error;
    error << "StkFrames::operator[]: invalid index (" << n << ") value!";
    Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  return data_[n];
}

inline StkFloat StkFrames :: operator[] ( size_t n ) const
{
#if defined(_STK_DEBUG_)
  if ( n >= size_ ) {
    std::ostringstream error;
    error << "StkFrames::operator[]: invalid index (" << n << ") value!";
    Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  return data_[n];
}

inline StkFloat& StkFrames :: operator() ( size_t frame, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( frame >= nFrames_ || channel >= nChannels_ ) {
    std::ostringstream error;
    error << "StkFrames::operator(): invalid frame (" << frame << ") or channel (" << channel << ") value!";
    Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  return data_[ frame * nChannels_ + channel ];
}

inline StkFloat StkFrames :: operator() ( size_t frame, unsigned int channel ) const
{
#if defined(_STK_DEBUG_)
  if ( frame >= nFrames_ || channel >= nChannels_ ) {
    std::ostringstream error;
    error << "StkFrames::operator(): invalid frame (" << frame << ") or channel (" << channel << ") value!";
    Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  return data_[ frame * nChannels_ + channel ];
}
    
inline StkFrames StkFrames::operator+(const StkFrames &f) const
{
#if defined(_STK_DEBUG_)
  if ( f.frames() != nFrames_ || f.channels() != nChannels_ ) {
    std::ostringstream error;
    error << "StkFrames::operator+: frames argument must be of equal dimensions!";
    Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif
  StkFrames sum((unsigned int)nFrames_,nChannels_);
  StkFloat *sumPtr = &sum[0];
  const StkFloat *fptr = f.data_;
  const StkFloat *dPtr = data_;
  for (unsigned int i = 0; i < size_; i++) {
    *sumPtr++ = *fptr++ + *dPtr++;
  }
  return sum;
}

inline void StkFrames :: operator+= ( StkFrames& f )
{
#if defined(_STK_DEBUG_)
  if ( f.frames() != nFrames_ || f.channels() != nChannels_ ) {
    std::ostringstream error;
    error << "StkFrames::operator+=: frames argument must be of equal dimensions!";
    Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  StkFloat *fptr = &f[0];
  StkFloat *dptr = data_;
  for ( unsigned int i=0; i<size_; i++ )
    *dptr++ += *fptr++;
}

inline void StkFrames :: operator*= ( StkFrames& f )
{
#if defined(_STK_DEBUG_)
  if ( f.frames() != nFrames_ || f.channels() != nChannels_ ) {
    std::ostringstream error;
    error << "StkFrames::operator*=: frames argument must be of equal dimensions!";
    Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  StkFloat *fptr = &f[0];
  StkFloat *dptr = data_;
  for ( unsigned int i=0; i<size_; i++ )
    *dptr++ *= *fptr++;
}

// Here are a few other useful typedefs.
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef signed short SINT16;
typedef signed int SINT32;
typedef float FLOAT32;
typedef double FLOAT64;

// The default sampling rate.
const StkFloat SRATE = 44100.0;

// The default real-time audio input and output buffer size.  If
// clicks are occuring in the input and/or output sound stream, a
// larger buffer size may help.  Larger buffer sizes, however, produce
// more latency.
const unsigned int RT_BUFFER_SIZE = 512;

// The default rawwave path value is set with the preprocessor
// definition RAWWAVE_PATH.  This can be specified as an argument to
// the configure script, in an integrated development environment, or
// below.  The global STK rawwave path variable can be dynamically set
// with the Stk::setRawwavePath() function.  This value is
// concatenated to the beginning of all references to rawwave files in
// the various STK core classes (ex. Clarinet.cpp).  If you wish to
// move the rawwaves directory to a different location in your file
// system, you will need to set this path definition appropriately.
#if !defined(RAWWAVE_PATH)
  #define RAWWAVE_PATH "../../rawwaves/"
#endif

const StkFloat PI           = 3.14159265358979;
const StkFloat TWO_PI       = 2 * PI;
const StkFloat ONE_OVER_128 = 0.0078125;

#if defined(__WINDOWS_DS__) || defined(__WINDOWS_ASIO__) || defined(__WINDOWS_MM__)
  #define __OS_WINDOWS__
  #define __STK_REALTIME__
#elif defined(__LINUX_OSS__) || defined(__LINUX_ALSA__) || defined(__UNIX_JACK__)
  #define __OS_LINUX__
  #define __STK_REALTIME__
#elif defined(__IRIX_AL__)
  #define __OS_IRIX__
#elif defined(__MACOSX_CORE__) || defined(__UNIX_JACK__)
  #define __OS_MACOSX__
  #define __STK_REALTIME__
#endif

static const StkFloat sinTable [259] = {
  -0.024541229009628296f, 0.000000000000000000f, 0.024541229009628296f,
  0.049067676067352295f, 0.073564566671848297f, 0.098017141222953796f,
  0.122410677373409270f, 0.146730467677116390f,
  0.170961886644363400f, 0.195090323686599730f, 0.219101235270500180f,
  0.242980182170867920f, 0.266712754964828490f, 0.290284663438797000f,
  0.313681751489639280f, 0.336889863014221190f,
  0.359895050525665280f, 0.382683426141738890f, 0.405241310596466060f,
  0.427555084228515630f, 0.449611335992813110f, 0.471396744251251220f,
  0.492898195981979370f, 0.514102756977081300f,
  0.534997642040252690f, 0.555570244789123540f, 0.575808167457580570f,
  0.595699310302734380f, 0.615231573581695560f, 0.634393274784088130f,
  0.653172850608825680f, 0.671558976173400880f,
  0.689540565013885500f, 0.707106769084930420f, 0.724247097969055180f,
  0.740951120853424070f, 0.757208824157714840f, 0.773010432720184330f,
  0.788346409797668460f, 0.803207516670227050f,
  0.817584812641143800f, 0.831469595432281490f, 0.844853579998016360f,
  0.857728600502014160f, 0.870086967945098880f, 0.881921291351318360f,
  0.893224298954010010f, 0.903989315032958980f,
  0.914209783077239990f, 0.923879504203796390f, 0.932992815971374510f,
  0.941544055938720700f, 0.949528157711029050f, 0.956940352916717530f,
  0.963776051998138430f, 0.970031261444091800f,
  0.975702106952667240f, 0.980785250663757320f, 0.985277652740478520f,
  0.989176511764526370f, 0.992479562759399410f, 0.995184719562530520f,
  0.997290432453155520f, 0.998795449733734130f,
  0.999698817729949950f, 1.000000000000000000f, 0.999698817729949950f,
  0.998795449733734130f, 0.997290432453155520f, 0.995184719562530520f,
  0.992479562759399410f, 0.989176511764526370f,
  0.985277652740478520f, 0.980785250663757320f, 0.975702106952667240f,
  0.970031261444091800f, 0.963776051998138430f, 0.956940352916717530f,
  0.949528157711029050f, 0.941544055938720700f,
  0.932992815971374510f, 0.923879504203796390f, 0.914209783077239990f,
  0.903989315032958980f, 0.893224298954010010f, 0.881921291351318360f,
  0.870086967945098880f, 0.857728600502014160f,
  0.844853579998016360f, 0.831469595432281490f, 0.817584812641143800f,
  0.803207516670227050f, 0.788346409797668460f, 0.773010432720184330f,
  0.757208824157714840f, 0.740951120853424070f,
  0.724247097969055180f, 0.707106769084930420f, 0.689540565013885500f,
  0.671558976173400880f, 0.653172850608825680f, 0.634393274784088130f,
  0.615231573581695560f, 0.595699310302734380f,
  0.575808167457580570f, 0.555570244789123540f, 0.534997642040252690f,
  0.514102756977081300f, 0.492898195981979370f, 0.471396744251251220f,
  0.449611335992813110f, 0.427555084228515630f,
  0.405241310596466060f, 0.382683426141738890f, 0.359895050525665280f,
  0.336889863014221190f, 0.313681751489639280f, 0.290284663438797000f,
  0.266712754964828490f, 0.242980182170867920f,
  0.219101235270500180f, 0.195090323686599730f, 0.170961886644363400f,
  0.146730467677116390f, 0.122410677373409270f, 0.098017141222953796f,
  0.073564566671848297f, 0.049067676067352295f,
  0.024541229009628296f, 0.000000000000000122f, -0.024541229009628296f,
  -0.049067676067352295f, -0.073564566671848297f, -0.098017141222953796f,
  -0.122410677373409270f, -0.146730467677116390f,
  -0.170961886644363400f, -0.195090323686599730f, -0.219101235270500180f,
  -0.242980182170867920f, -0.266712754964828490f, -0.290284663438797000f,
  -0.313681751489639280f, -0.336889863014221190f,
  -0.359895050525665280f, -0.382683426141738890f, -0.405241310596466060f,
  -0.427555084228515630f, -0.449611335992813110f, -0.471396744251251220f,
  -0.492898195981979370f, -0.514102756977081300f,
  -0.534997642040252690f, -0.555570244789123540f, -0.575808167457580570f,
  -0.595699310302734380f, -0.615231573581695560f, -0.634393274784088130f,
  -0.653172850608825680f, -0.671558976173400880f,
  -0.689540565013885500f, -0.707106769084930420f, -0.724247097969055180f,
  -0.740951120853424070f, -0.757208824157714840f, -0.773010432720184330f,
  -0.788346409797668460f, -0.803207516670227050f,
  -0.817584812641143800f, -0.831469595432281490f, -0.844853579998016360f,
  -0.857728600502014160f, -0.870086967945098880f, -0.881921291351318360f,
  -0.893224298954010010f, -0.903989315032958980f,
  -0.914209783077239990f, -0.923879504203796390f, -0.932992815971374510f,
  -0.941544055938720700f, -0.949528157711029050f, -0.956940352916717530f,
  -0.963776051998138430f, -0.970031261444091800f,
  -0.975702106952667240f, -0.980785250663757320f, -0.985277652740478520f,
  -0.989176511764526370f, -0.992479562759399410f, -0.995184719562530520f,
  -0.997290432453155520f, -0.998795449733734130f,
  -0.999698817729949950f, -1.000000000000000000f, -0.999698817729949950f,
  -0.998795449733734130f, -0.997290432453155520f, -0.995184719562530520f,
  -0.992479562759399410f, -0.989176511764526370f,
  -0.985277652740478520f, -0.980785250663757320f, -0.975702106952667240f,
  -0.970031261444091800f, -0.963776051998138430f, -0.956940352916717530f,
  -0.949528157711029050f, -0.941544055938720700f,
  -0.932992815971374510f, -0.923879504203796390f, -0.914209783077239990f,
  -0.903989315032958980f, -0.893224298954010010f, -0.881921291351318360f,
  -0.870086967945098880f, -0.857728600502014160f,
  -0.844853579998016360f, -0.831469595432281490f, -0.817584812641143800f,
  -0.803207516670227050f, -0.788346409797668460f, -0.773010432720184330f,
  -0.757208824157714840f, -0.740951120853424070f,
  -0.724247097969055180f, -0.707106769084930420f, -0.689540565013885500f,
  -0.671558976173400880f, -0.653172850608825680f, -0.634393274784088130f,
  -0.615231573581695560f, -0.595699310302734380f,
  -0.575808167457580570f, -0.555570244789123540f, -0.534997642040252690f,
  -0.514102756977081300f, -0.492898195981979370f, -0.471396744251251220f,
  -0.449611335992813110f, -0.427555084228515630f,
  -0.405241310596466060f, -0.382683426141738890f, -0.359895050525665280f,
  -0.336889863014221190f, -0.313681751489639280f, -0.290284663438797000f,
  -0.266712754964828490f, -0.242980182170867920f,
  -0.219101235270500180f, -0.195090323686599730f, -0.170961886644363400f,
  -0.146730467677116390f, -0.122410677373409270f, -0.098017141222953796f,
  -0.073564566671848297f, -0.049067676067352295f,
  -0.024541229009628296f, -0.000000000000000245f, 0.024541229009628296f
};

#define TABLE_SIZE2 256
/**
 * @brief  Fast approximation to the trigonometric sine function for floating-point data.
 * @param[in] x input value in radians.
 * @return  sin(x).
 */

inline StkFloat fastSin(StkFloat x)
{
	StkFloat sinVal, fract, in;                   /* Temporary variables for input, output */
	StkFloat index;                                /* Index variables */
  StkFloat tableSize = (int) TABLE_SIZE2;    /* Initialise tablesize */
  StkFloat wa, wb, wc, wd;                      /* Cubic interpolation coefficients */
  StkFloat a, b, c, d;                          /* Four nearest output values */
  StkFloat *tablePtr;                           /* Pointer to table */
  int n;

  /* input x is in radians */
  /* Scale the input to [0 1] range from [0 2*PI] , divide input by 2*pi */
  in = x * 0.159154943092f;

  /* Calculation of floor value of input */
  n = (int) in;

  /* Make negative values towards -infinity */
  if(x < 0.0f)
  {
    n = n - 1;
  }

  /* Map input value to [0 1] */
  in = in - (StkFloat) n;

  /* Calculation of index of the table */
  index = (int) (tableSize * in);

  /* fractional value calculation */
  fract = ((StkFloat) tableSize * in) - (StkFloat) index;

  /* Initialise table pointer */
  tablePtr = (StkFloat *) & sinTable[(int)index];

  /* Read four nearest values of output value from the sin table */
  a = *tablePtr++;
  b = *tablePtr++;
  c = *tablePtr++;
  d = *tablePtr++;

  /* Cubic interpolation process */
  wa = -(((0.166666667f) * (fract * (fract * fract))) +
        ((0.3333333333333f) * fract)) + ((0.5f) * (fract * fract));
  wb = (((0.5f) * (fract * (fract * fract))) -
       ((fract * fract) + ((0.5f) * fract))) + 1.0f;
  wc = (-((0.5f) * (fract * (fract * fract))) +
       ((0.5f) * (fract * fract))) + fract;
  wd = ((0.166666667f) * (fract * (fract * fract))) -
       ((0.166666667f) * fract);

  /* Calculate sin value */
  sinVal = ((a * wa) + (b * wb)) + ((c * wc) + (d * wd));

  /* Return the output value */
  return (sinVal);

}

inline double fastPow(double a, double b) {
  union {
    double d;
    int x[2];
  } u = { a };
  u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
  u.x[0] = 0;
  return u.d;
}


inline StkFloat _sin(StkFloat in)
{
	return fastSin(in);
}

inline StkFloat _cos(StkFloat in)
{
	return std::cos(in);
}

inline StkFloat _exp(StkFloat in)
{
	return std::exp(in);
}

inline StkFloat _atan2(StkFloat in,StkFloat in2)
{
	return std::atan2(in,in2);
}

inline StkFloat _pow(StkFloat in,StkFloat in2)
{
	return fastPow(in,in2);
}

inline StkFloat _fmod(StkFloat in,StkFloat in2)
{
	return std::fmod(in,in2);
}

inline StkFloat _abs(StkFloat in)
{
	return std::abs(in);
}

inline StkFloat _fabs(StkFloat in)
{
	return std::fabs(in);
}

inline StkFloat _log(StkFloat in)
{
	return std::log(in);
}

inline StkFloat _floor(StkFloat in)
{
	return std::floor(in);
}

inline StkFloat _sqrt(StkFloat in)
{
	return std::sqrt(in);
}

inline StkFloat _ceil(StkFloat in)
{
	return std::ceil(in);
}



} // stk namespace


#endif

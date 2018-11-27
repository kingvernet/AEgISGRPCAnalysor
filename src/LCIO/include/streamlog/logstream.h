// -*- mode: c++;
#ifndef logstream_h
#define logstream_h

#include "streamlog/prefix.h"

#include <iostream>
#include <map>

namespace streamlog{

  class prefix_base ;
  class logbuffer ;
  class logscope ;


  /** Class defining a log stream that is used to print log messages depending 
   *  on current log threshold. Can be initialized with any std::ostream, typically either 
   *  std::cout or an std::ofstream file handler.
   *  There is one global instance of this class defined in the library: logstream::out  <br>
   *  Typically only this instance is needed, e.g.: <br>
   *  <pre>
   *    // in int main() :
   *    streamlog::out.init( std::cout, argv[0] ) ;
   * 
   *    //...
   *    
   *    if( streamlog::out.write< streamlog::DEBUG1 >() )
   *       streamlog::out() << " this message will only be printed if threshold <= DEBUG1::level " 
   *                        << std::endl ;
   * 
   *    // or the same, simply using a macro:
   * 
   *    streamlog_out( DEBUG )  << " this message will only be printed if threshold <= DEBUG1::level " 
   *                            << std::endl ;
   * 
   *    
   *  </pre>
   *  Note that with the above calling sequence or the macro no runtime overhead is created
   *  if streamlog::DEBUG1::active is false and else if the log threshold is larger than 
   *  streamlog::DEBUG1::level no formatting of the message will happen, i.e. also very little 
   *  runtime cost is involved. <br>
   *  the behaviour of the logstream, i.e. the current log threshold and log scope name can be changed only
   *  through an object of streamlog::logscope.
   * 
   *  @see logstream::write() 
   *  @see logscope
   *
   *  @author F. Gaede, DESY
   *  @version $Id$
   */
  class logstream {

    friend class logscope ;
    friend class logbuffer ;


    typedef std::map< std::string,  unsigned > LevelMap ;

  public :

    logstream() ; 

    ~logstream() ;

    /** Initialize the logstream with an std::ostream, e.g. std::cout and 
     *  the main scope name, e.g. argv[0]. The default log level names of stream log are 
     *  added, so that the the threshhold can be set with a string (unless addDefaultNames=false).
     *  Only first call to this method has an effect, subsequent calls are ignored.
     * 
     */
    void init( std::ostream& os , const std::string name, bool addDefaultNames=true) ;

    /** True if next log message of the current level (class T ) will be written, i.e.
     *  the next call to std::ostream& operator()() will return a valid outstream.
     * 
     */
    template<class T>
    inline bool write() {
      
      // dont' call chek_level if T::active == false
      return (  T::active   &&    check_level<T>()  ) ;
    }

    /** True if next log message of the current level (class T ) would be written
     *  - can be used to conditionally execute code blocks that are needed before writing to
     * the outstream.
     * 
     */
    template<class T>
    inline bool would_write() {
      
      return (  T::active   &&  T::level >= _threshold ) ;
    }

   /** Return the actual std::ostream where the message should be writen to - will return
     *  a nullstream unless prepended by a successfull call to write<MESSAGELEVEL>()
     */
    std::ostream& operator()() ;

    /** Adds a level name to the stream for setting the log threshold with a string through 
     *  a scope class. Only names added with this method will have an effect - other log 
     *  thresholds can only be activated with logstream::logscope::setThreshold<T>().
     *  Useful for framework programs where the log threshold can be changed via a steering 
     *  parameter.
     *  
     *  @see logstream::logscope::setThreshold(const std::string levelName )
     */
    template <class T>
    void addLevelName() {

      _map[ T::name() ] = T::level ;
    }

  protected:

    /** Set the current threshold - user need to use a streamlog::logscope object 
     *  to do this.
     */
    void setThreshold( unsigned level ) { _threshold = level ; } 

    /** Set the current threshold through its name - only level previously made known to
     *  the stream through addLevelName will have an effect.
     */
    unsigned setThreshold( const std::string& levelName )  ; 

    /** Returns the prefix for the logbuffer object */
    prefix_base* prefix() { return _prefix ; }

    /** used internally by write<T> */
    template<class T>
    bool check_level() {
      
      if( T::level >= _threshold ){
	_active = true ;
	_prefix->_levelName = T::name() ;
      }
      return _active ;
    }

    /** Adds the default streamlog lovel names to this logstream:
     *	DEBUG0-DEBUG9, MESSAGE0-MESSAGE9, WARNING0-WARNING9, ERROR0-ERROR9.
     */
    void addDefaultLevelNames() ;


  private:

    /** Private helper class returned if message log threshold not reached */ 
    class nullstream :  public std::ostream {
    public:
      nullstream() : std::ios( 0 ), std::ostream( 0 ) {} ;
    } ;
  

    nullstream* _ns ;       // the nullstream
    std::ostream* _os ;     // wrapper for actual ostream
    unsigned _threshold ;   // current log threshold 
    bool _active ;          // boolean helper 
    logbuffer* _lb ;        // log buffer adds prefix to everu log message
    prefix_base* _prefix ;  // prefix formatter
    LevelMap _map ;         // string map of level names
    
  } ;

  /** Global instance of the default logstream */
  extern logstream out ;

}
#endif

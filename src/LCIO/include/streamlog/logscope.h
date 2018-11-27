#ifndef logscope_h
#define logscope_h

#include "streamlog/logstream.h"

namespace streamlog{

  /** Helper class to change the current scope name and log threshold - 
   *  if the object goes out of scope, e.g. at the end of the method
   *  where it is instantiated scope name and log threshold will be reset to the values 
   *  they had previously, i.e. before changed through this object.<br>
   *  Example:
   *  <pre>
   *   streamlog::logscope scope( streamlog::out ) ;
   *   scope.setName( "TrackFitter" ) ;
   *   scope.setThreshold< streamlog::MESSAGE3 >() ;
   *  </pre>
   *  
   * 
   *  @author F. Gaede, DESY
   *  @version $Id$
   */
  class logscope{

  public:
    /** Instantiate a scope object for the given logstream.
     */
    logscope(logstream& ls) : _ls(&ls) , _name("") , _threshold(-1) { }
    
    /** Reset old name and threshold if set through this object.
     */
    ~logscope(){

      if( _name.size() > 0 ) {
	_ls->prefix()->_name=_name ;
	//std::cerr << "  ~logscope()  reset name to " << _name << std::endl ;
	
      }
      if(  _threshold > -1 )
 	_ls->setThreshold( _threshold ) ; 
    }
    
    /** Change current log scope name for the lifetime of this object */
    void setName(const std::string name) {
      _name =  _ls->prefix()->_name ;
      _ls->prefix()->_name = name ;
    }
  

    /** Change current log threshold for the lifetime of this object */
    template <class T>
    void setThreshold(){
      _threshold =  _ls->_threshold ;
      _ls->setThreshold( T::level )  ;
    }

    /** Change current log threshold for the lifetime of this object 
     *  through a string level name - if knwon to the logstream, 
     *  otherwise the call will have no effect.
     */
    void setThreshold(const std::string& level){
      _threshold =  _ls->setThreshold( level )  ;
    }
   
  protected: 
    logstream* _ls ;
    std::string _name ;
    int _threshold;
    logscope() {}
    
  };
  
}
#endif

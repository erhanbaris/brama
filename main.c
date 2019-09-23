#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>

#include "static_py.h"

int main()
{
    void* context = static_py_init();
    //static_py_execute(context, "do  if  in  for  let  new  try  var  case  else  enum  eval  null  this  true  void  with  break  catch  class  const  false  super  throw  while  yield  delete  export  import  public  return  static  switch  typeof  default  extends  finally  package  private  continue  debugger  function  arguments  interface  protected  implements  instanceof");
    static_py_execute(context, "'erhan baris 12345'");
    static_py_destroy(context);

    return 0;
}

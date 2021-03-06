#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

#ifdef ENABLE_DEBUG
#define DEBUG(expr) expr
#else
#define DEBUG(expr) 
#endif

//disable some output code 
//dont use comment because consider code may used in future
//but comment may be delete sometimes
#define DISABLE(expr) 

#define RET_ON_FAIL(cond) if(!(cond)){outs()<<"Failed at:"<<__LINE__<<"\n";return NULL;}
#define VERBOSE(expr,verb) (expr || (outs()<<"<<HERE>>:"<<*verb<<"\n",0))

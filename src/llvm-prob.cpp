#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/PassManager.h>
#include <llvm/Bitcode/ReaderWriter.h>

#include <llvm/Support/PrettyStackTrace.h>
#include <llvm/Support/Signals.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/system_error.h>
#include <llvm/Support/raw_ostream.h>

#include <llvm/Analysis/BlockFrequencyInfo.h>

using namespace llvm;

namespace {
  cl::opt<std::string>
  BitcodeFile(cl::Positional, cl::desc("<program bitcode file>"),
              cl::Required);
};

namespace {
    class FrequencyPrintPass:public FunctionPass 
    {
        static char ID;
        public:
        explicit FrequencyPrintPass(): FunctionPass(ID) {}
        void getAnalysisUsage(AnalysisUsage &AU) const 
        {
            AU.setPreservesAll();
            AU.addRequired<BlockFrequencyInfo>();
        }
        bool runOnFunction(Function& F)
        {
            outs()<<"Function:"<<F.getName();
            BlockFrequencyInfo& BFI = getAnalysis<BlockFrequencyInfo>();
            BFI.print(outs(), F.getParent());
            outs()<<"----------------------\n";

            return 0;
        }
    };
};

char FrequencyPrintPass::ID = 0;

int main(int argc, char **argv) {
  // Print a stack trace if we signal out.
  sys::PrintStackTraceOnErrorSignal();
  PrettyStackTraceProgram X(argc, argv);

  LLVMContext &Context = getGlobalContext();
  llvm_shutdown_obj Y;  // Call llvm_shutdown() on exit.
  
  cl::ParseCommandLineOptions(argc, argv, "llvm profile dump decoder\n");

  // Read in the bitcode file...
  std::string ErrorMessage;
  OwningPtr<MemoryBuffer> Buffer;
  error_code ec;
  Module *M = 0;
  if (!(ec = MemoryBuffer::getFileOrSTDIN(BitcodeFile, Buffer))) {
    M = ParseBitcodeFile(Buffer.get(), Context, &ErrorMessage);
  } else
    ErrorMessage = ec.message();
  if (M == 0) {
    errs() << argv[0] << ": " << BitcodeFile << ": "
      << ErrorMessage << "\n";
    return 1;
  }

  // Read the profiling information. This is redundant since we load it again
  // using the standard profile info provider pass, but for now this gives us
  // access to additional information not exposed via the ProfileInfo
  // interface.

  FunctionPassManager f_pass_mgr(M);
  f_pass_mgr.add(new BlockFrequencyInfo());
  f_pass_mgr.add(new FrequencyPrintPass());

  for( auto& func : *M ){
      f_pass_mgr.run(func);
  }


  return 0;
}

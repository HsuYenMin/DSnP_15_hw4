/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "../util/util.h"

using namespace std;
extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}

bool
myStr2Size_t(const string& str, size_t& num)
{
   num = 0;
   size_t i = 0;
   bool valid = false;
   for (; i < str.size(); ++i) {
      if (isdigit(str[i])) {
         num *= 10;
         num += int(str[i] - '0');
         valid = true;
      }
      else return false;
   }
   return valid;
}

//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < toSizeT(sizeof(MemTestObj))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{
   cout << setw(15) << left << "MTReset: "
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   //check option
   if(option.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   vector<string> options;
   if(!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;
   size_t size_tTemp, arrsize;
   if(options.size() == 1){
      if(myStr2Size_t(options[0], size_tTemp)){
         mtest.newObjs(size_tTemp);
      }
      else if(!myStrNCmp("-Array", options[0], 2))
         return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);
      else
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   }
   else if(options.size() == 2){
      if(myStrNCmp("-Array", options[0], 2) && !myStr2Size_t(options[0], size_tTemp))
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
      else if(myStr2Size_t(options[0], size_tTemp)){
         if(myStrNCmp("-Array", options[1], 2))
            return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);
         else
            return CmdExec::errorOption(CMD_OPT_EXTRA, options[1]);
      }
      else
         return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);
   }
   else if(options.size() == 3){
      if(myStrNCmp("-Array", options[0], 2) && !myStr2Size_t(options[0], size_tTemp))
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
      else if(myStr2Size_t(options[0], size_tTemp)){
         if(myStrNCmp("-Array", options[1], 2))
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
         else{
            if(myStr2Size_t(options[2], arrsize))
               mtest.newArrs(size_tTemp, arrsize);
            else
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
         }
      }
      else{
         if(myStr2Size_t(options[1], arrsize)){
            if(myStr2Size_t(options[2], size_tTemp))
               mtest.newArrs(size_tTemp, arrsize);
            else
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
         }
         else
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
      }
   }
   else
      return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);

   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{
   cout << setw(15) << left << "MTNew: "
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   if(option.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   vector<string> options;
   if(!CmdExec::lexOptions(option, options))
         return CMD_EXEC_ERROR;
   size_t objId, numRandId;
   if(options.size() == 1){
      if(!myStrNCmp("-Index", options[0], 2) || !myStrNCmp("-Random", options[0], 2)
         || !myStrNCmp("-Array", options[0], 2))
         return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);
      else
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   }
   else if(options.size() == 2){
      if(!myStrNCmp("-Index", options[0], 2)){
         if(myStr2Size_t(options[1], objId)){
            if(objId < mtest.getObjListSize())
               mtest.deleteObj(objId);
            else{
               cerr << "objId (" << objId << ") is out of range!" << endl;
               return CMD_EXEC_ERROR;
               }
         }
         else
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
      }
      else if(!myStrNCmp("-Random", options[0], 2)){
         if(myStr2Size_t(options[1], numRandId)){
            if(mtest.getObjListSize()){
               for(size_t i = 0; i < numRandId; i++){
                  objId = rnGen(mtest.getObjListSize());
                  if(objId < mtest.getObjListSize())
                     mtest.deleteObj(objId);
               }
            }
            else{
               cerr << "ObjList is still empty!" << endl;
               return CMD_EXEC_ERROR;
            }
         }
         else
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
      }
      else if(!myStrNCmp("-Array", options[0], 2)){
         if(!myStrNCmp("-Index", options[1], 2)
            || !myStrNCmp("-Random", options[1], 2))
            return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);
         else
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
      }
      else
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   }
   else if(options.size() == 3){
      if(!myStrNCmp("-Array", options[0], 2)){
         if(!myStrNCmp("-Index", options[1], 2)){
            if(myStr2Size_t(options[2], objId)){
               if(objId < mtest.getObjListSize())
                  mtest.deleteObj(objId);
               else{
                  cerr << "objId (" << objId << ") is out of range!" << endl;
                  return CMD_EXEC_ERROR;
               }
            }
            else
               return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
         }
         else if(!myStrNCmp("-Random", options[1], 2)){
            if(mtest.getArrListSize()){
               if(myStr2Size_t(options[2], numRandId)){
                  for(size_t i = 0; i < numRandId; i++){
                     objId = rnGen(mtest.getArrListSize());
                     if(objId < mtest.getArrListSize())
                        mtest.deleteArr(objId);
                  }
               }
               else
                  return CMD_EXEC_ERROR;
            }
            else{
               cerr << "ArrList is still empty!" << endl;
               return CMD_EXEC_ERROR;
            }
         }
      }
      else if(!myStrNCmp("-Index", options[0], 2)){
         if(myStr2Size_t(options[1], objId)){
            if(!myStrNCmp("-Array", options[2], 2)){
               if(objId < mtest.getArrListSize())
                  mtest.deleteArr(objId);
               else{
                  cerr << "arrId (" << objId << ") is out of range!" << endl;
                  return CMD_EXEC_ERROR;
               }
            }
            else
               return CmdExec::errorOption(CMD_OPT_EXTRA, options[2]);
         }
         else
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
      }
      else if(!myStrNCmp("-Random", options[0], 2)){
         if(mtest.getArrListSize()){
            if(myStr2Size_t(options[1], numRandId)){
               if(!myStrNCmp("-Array", options[2], 2)){
                  for(size_t i = 0; i < numRandId; i++){
                     objId = rnGen(mtest.getArrListSize());
                     if(objId < mtest.getArrListSize())
                        mtest.deleteArr(objId);
                  }
               }
               else
                  return CmdExec::errorOption(CMD_OPT_EXTRA, options[2]); 
            }
            else
               return CMD_EXEC_ERROR;//cerr ?
         }
         else{
            cerr << "ArrList is still empty!" << endl;
            return CMD_EXEC_ERROR;
         }
      }
      else
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
   }
   else
      return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{
   cout << setw(15) << left << "MTDelete: "
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{
   cout << setw(15) << left << "MTPrint: "
        << "(memory test) print memory manager info" << endl;
}

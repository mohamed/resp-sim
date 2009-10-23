/***************************************************************************\
 *
 *
 *         ___           ___           ___           ___
 *        /  /\         /  /\         /  /\         /  /\
 *       /  /::\       /  /:/_       /  /:/_       /  /::\
 *      /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
 *     /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
 *    /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
 *    \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
 *     \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
 *      \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
 *       \  \:\        \  \::/        /__/:/       \  \:\
 *        \__\/         \__\/         \__\/         \__\/
 *
 *
 *
 *
 *   This file is part of ReSP.
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Giovanni Beltrame, Luca Fossati
 *       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
 *
\***************************************************************************/

#include <vector>
#include <map>

#include <systemc.h>

#include "concurrency_manager.hpp"

//Initialization of some static variables
const int resp::ConcurrencyManager::SYSC_SCHED_FIFO = 0;
const int resp::ConcurrencyManager::SYSC_SCHED_OTHER = 1;
const int resp::ConcurrencyManager::SYSC_SCHED_RR = 2;
const int resp::ConcurrencyManager::SYSC_SCHED_EDF = 5;

const int resp::ConcurrencyManager::SYSC_PRIO_MAX = 255;
const int resp::ConcurrencyManager::SYSC_PRIO_MIN = 0;

const int resp::ConcurrencyManager::SYSC_PREEMPTIVE = 1;
const int resp::ConcurrencyManager::SYSC_NON_PREEMPTIVE = 0;

///specifies whether blocked processor halts or keep on working in busy wait loops
bool resp::ConcurrencyManager::busyWaitLoop = true;
///Specifies the stack size for each thread
unsigned int resp::ConcurrencyManager::threadStackSize = 1024*20;
///Associates thred properties with a routine name: all threads
///created from that routine shall have such properties
std::map<std::string, DefaultThreadInfo> resp::ConcurrencyManager::defThreadInfo;
///The registered interrupt service routines.
std::map<int, std::string> resp::ConcurrencyManager::interruptServiceRoutines;
///The size and content of the thread-local-storage
unsigned int resp::ConcurrencyManager::tlsSize = 0;
unsigned char * resp::ConcurrencyManager::tlsData = NULL;

resp::ConcurrencyManager::ConcurrencyManager(){
    //Constructor: there is not much to do
    //a part from se-setting the reentracy
    //variables
    this->mallocMutex = -1;
    this->sfpMutex = -1;
    this->sinitMutex = -1;
    this->fpMutex = -1;
    this->maxProcId = 0;
}

///Resets the CM to its original status as after the construction
void resp::ConcurrencyManager::reset(){
    //Resetting static variables
    resp::ConcurrencyManager::busyWaitLoop = true;
    resp::ConcurrencyManager::threadStackSize = 1024*20;
    resp::ConcurrencyManager::defThreadInfo.clear();
    resp::ConcurrencyManager::interruptServiceRoutines.clear();
    resp::ConcurrencyManager::tlsSize = 0;
    resp::ConcurrencyManager::tlsData = NULL;
    //Resetting instance variables
    this->mallocMutex = -1;
    this->sfpMutex = -1;
    this->sinitMutex = -1;
    this->fpMutex = -1;
    this->maxProcId = 0;
}

///*******************************************************************
/// Initialization functions
///*******************************************************************
///Initializes the mutexes used to guarantee mutual exclusion access to
///reentrant routines
void resp::ConcurrencyManager::initReentrantEmulation(){
    this->mallocMutex = this->createMutex();
    this->sfpMutex = this->createMutex();
    this->sinitMutex = this->createMutex();
    this->fpMutex = this->createMutex();
}

///*******************************************************************
/// Here are some functions for computing statistics on the
/// execution
///*******************************************************************
void resp::ConcurrencyManager::printThreadTraces(){
    THROW_EXCEPTION(__PRETTY_FUNCTION__ << " not yet implemented");
}
std::vector<std::string> resp::ConcurrencyManager::getThreadTraces(){
    std::vector<std::string> traces;
    THROW_EXCEPTION(__PRETTY_FUNCTION__ << " not yet implemented");
    return traces;
}
std::map<int, double> resp::ConcurrencyManager::getAverageLoads(){
    std::map<int, double> avgLoad;
    THROW_EXCEPTION(__PRETTY_FUNCTION__ << " not yet implemented");
    return avgLoad;
}
void resp::ConcurrencyManager::printIdleTraces(){
    THROW_EXCEPTION(__PRETTY_FUNCTION__ << " not yet implemented");
}
std::vector<std::string> resp::ConcurrencyManager::getIdleTraces(){
    std::vector<std::string> idleTrace;
    THROW_EXCEPTION(__PRETTY_FUNCTION__ << " not yet implemented");
    return idleTrace;
}
int resp::ConcurrencyManager::getDeadlineMisses(){
    THROW_EXCEPTION(__PRETTY_FUNCTION__ << " not yet implemented");
    return 0;
}
int resp::ConcurrencyManager::getDeadlineOk(){
    THROW_EXCEPTION(__PRETTY_FUNCTION__ << " not yet implemented");
    return 0;
}

///*******************************************************************
/// Finally we have the real core of the concurrency manager,
/// it contains the functions emulating the functionalities of
/// the emulated concurrency related primitives
///*******************************************************************
///*********** Thread related routines *******************
int resp::ConcurrencyManager::createThread(unsigned int threadFun, unsigned int args, int attr){
    //I have to create the thread handle and add it to the list of
    //existing threads; in case there is an available processor,
    //I also start the execution of the thread on it; note that all the
    //data structures touched here are also used during scheduling,
    //so synchronization must be enforced with respect to those structures

    this->schedLock.lock();
    //First of all lets determine the stack size
/*    unsigned int curStackBase = 0;
    unsigned int curStackSize = 0;
    if(existingAttr.find(attr) == existingAttr.end()){
        curStackSize = defaultStackSize;
    }
    else{
        curStackSize = existingAttr[attr]->stackSize;
    }
    //I have to determine if there is space for
    //my stack among two already existing ones
    //or if I have to position myself at the end of
    //the stack
    std::map<unsigned int, unsigned int>::iterator stacksIter, stacksNext, stacksEnd;
    for(stacksIter = stacks.begin(), stacksNext = stacksIter, stacksNext++, stacksEnd = stacks.end();
                                                                        stacksNext != stacksEnd; stacksIter++, stacksNext++){
        if((stacksIter->first - stacksIter->second - stacksNext->first) > curStackSize){
            curStackBase = stacksIter->first - stacksIter->second;
            stacks[curStackBase] = curStackSize;
            break;
        }
    }
    if(curStackBase == 0){
        //I haven't found any suitable stack
        curStackBase = stacksIter->first - stacksIter->second;
        stacks[curStackBase] = curStackSize;
    }
    if(curStackBase < archc::ac_heap_ptr)
        THROW_EXCEPTION("Unable to allocate " << curStackSize << " bytes for the stack of thread " << existingThreads.size() << ": no more space in memory");
    ThreadEmu *th = NULL;
    AttributeEmu *curAttr = NULL;
    std::string routineName = bfdFE->symbolAt((unsigned long)threadFun).front();
    if(existingAttr.find(attr) == existingAttr.end()){
        if(defThreadInfo.find(routineName) == defThreadInfo.end()){
            th = new ThreadEmu(existingThreads.size(), threadFun, args, curStackBase, defaultTLSSize, &defaultAttr);
        }
        else{
            curAttr = existingAttr[createThreadAttr()];
            curAttr->preemptive = defThreadInfo[routineName].preemptive;
            curAttr->schedPolicy = defThreadInfo[routineName].schedPolicy;
            curAttr->priority = defThreadInfo[routineName].priority;
            curAttr->deadline = defThreadInfo[routineName].deadline;
            th = new ThreadEmu(existingThreads.size(), threadFun, args, curStackBase, defaultTLSSize, curAttr);
        }
    }
    else{
        curAttr = existingAttr[attr];
        if(defThreadInfo.find(routineName) != defThreadInfo.end()){
            curAttr->preemptive = defThreadInfo[routineName].preemptive;
            curAttr->schedPolicy = defThreadInfo[routineName].schedPolicy;
            curAttr->priority = defThreadInfo[routineName].priority;
            curAttr->deadline = defThreadInfo[routineName].deadline;
        }
        th = new ThreadEmu(existingThreads.size(), threadFun, args, curStackBase, defaultTLSSize, curAttr);
    }
    existingThreads[existingThreads.size()] = th;

    //I schedule the thread if I can find a free processor
    #ifndef NDEBUG
    std::cerr << "Trying to schedule thread " << existingThreads.size() - 1 << " on a free processor" << std::endl;
    #endif
    if(!scheduleFreeProcessor(existingThreads[existingThreads.size() - 1])){
        //Ok, I haven't found any free processor; I try to determine
        //if there is a processor running a lower priority thread and, in case,
        //I replace it
        #ifndef NDEBUG
        std::cerr << "Trying to schedule thread " << existingThreads.size() - 1 << " through preemption" << std::endl;
        #endif
        if(!preemptLowerPrio(existingThreads[existingThreads.size() - 1])){
            #ifndef NDEBUG
            std::cerr << "Unable to preempt any thread, adding " << existingThreads.size() - 1 << " to the ready queue" << std::endl;
            #endif
            //Ok, nothing else I can do: lets add the thread to the correct
            //ready queue
            int curPrio = 0;
            if(curAttr != NULL){
                if(curAttr->schedPolicy == resp::SYSC_SCHED_EDF)
                    curPrio = resp::SYSC_PRIO_MAX + 1;
                else
                    curPrio = curAttr->priority;
            }
            #ifndef NDEBUG
            if(curPrio < resp::SYSC_PRIO_MIN || curPrio > resp::SYSC_PRIO_MAX + 1){
                THROW_EXCEPTION("Non valid priority value " << curPrio);
            }
            #endif
            readyQueue[curPrio].push_back(existingThreads[existingThreads.size() - 1]);
            if(curPrio == resp::SYSC_PRIO_MAX + 1)
                sort(readyQueue[curPrio].begin(), readyQueue[curPrio].end(), deadlineSort);
        }
    }

    #ifndef NDEBUG
    std::cerr << "Created thread Id = " << existingThreads.size() - 1 << \
                " stack size = " << curStackSize/1024 << "KB" << " stack base " << \
                std::showbase << std::hex << curStackBase << std::dec << " status " << \
                th->status << " address " << std::hex << std::showbase << threadFun <<  std::endl;
    #endif

    schedulingLockBusy = false;
    schedulingEvent.notify();

    return (existingThreads.size() - 1);*/
}
void resp::ConcurrencyManager::exitThread(unsigned int procId, unsigned int retVal){
}
bool resp::ConcurrencyManager::cancelThread(int threadId){
}

int resp::ConcurrencyManager::createThreadAttr(){
}
void resp::ConcurrencyManager::deleteThreadAttr(int attr){
}

void resp::ConcurrencyManager::setStackSize(int attr, int stackSize){
}
unsigned int resp::ConcurrencyManager::getStackSize(int attr){
}

void resp::ConcurrencyManager::setPreemptive(int attr, int isPreemptive){
}
int resp::ConcurrencyManager::getPreemptive(int attr){
}

void resp::ConcurrencyManager::setSchedDeadline(int attr, unsigned int deadline){
}
unsigned int resp::ConcurrencyManager::getSchedDeadline(int attr){
}

void resp::ConcurrencyManager::setSchedPrio(int attr, int priority){
}
int resp::ConcurrencyManager::getSchedPrio(int attr){
}

void resp::ConcurrencyManager::setSchedPolicy(int attr, int policy){
}
int resp::ConcurrencyManager::getSchedPolicy(int attr){
}

int resp::ConcurrencyManager::getThreadSchePolicy(int threadId){
}
void resp::ConcurrencyManager::setThreadSchePolicy(int threadId, int policy){
}

int resp::ConcurrencyManager::getThreadSchedPrio(int threadId){
}
void resp::ConcurrencyManager::setThreadSchedPrio(int threadId, int priority){
}

unsigned int resp::ConcurrencyManager::getThreadSchedDeadline(int threadId){
}
void resp::ConcurrencyManager::setThreadSchedDeadline(int threadId, unsigned int deadline){
}

int resp::ConcurrencyManager::getThreadId(unsigned int procId){
}

int resp::ConcurrencyManager::createKey(){
}
void resp::ConcurrencyManager::setSpecific(unsigned int procId, int key, unsigned int memArea){
}
unsigned int resp::ConcurrencyManager::getSpecific(unsigned int procId, int key){
}

void resp::ConcurrencyManager::join(int thId, unsigned int procId, int curThread_){
}
void resp::ConcurrencyManager::joinAll(unsigned int procId){
}

std::pair<unsigned int, unsigned int> resp::ConcurrencyManager::readTLS(unsigned int procId){
}
void resp::ConcurrencyManager::idleLoop(unsigned int procId){
}

void resp::ConcurrencyManager::pushCleanupHandler(unsigned int procId, unsigned int routineAddress, unsigned int arg){
}
void resp::ConcurrencyManager::popCleanupHandler(unsigned int procId, bool execute){
}
void resp::ConcurrencyManager::execCleanupHandlerTop(unsigned int procId){
}

///*********** Mutex related routines *******************
///Destroys a previously allocated mutex, exception if the mutex does not exist
void resp::ConcurrencyManager::destroyMutex(unsigned int procId, int mutex){
}
int resp::ConcurrencyManager::createMutex(){
}
///Locks the mutex, deschedules the thread if the mutex is busy; in case
///nonbl == true, the function behaves as a trylock
bool resp::ConcurrencyManager::lockMutex(int mutex, unsigned int procId, bool nonbl){
}
///Releases the lock on the mutex
int resp::ConcurrencyManager::unLockMutex(int mutex, unsigned int procId){
}

///*********** Semaphore related routines *******************
int resp::ConcurrencyManager::createSem(unsigned int procId, int initialValue){
}
void resp::ConcurrencyManager::destroySem(unsigned int procId, int sem){
}
void resp::ConcurrencyManager::postSem(int sem, unsigned int procId){
}
void resp::ConcurrencyManager::waitSem(int sem, unsigned int procId){
}

///*********** Condition Variable related routines *******************
int resp::ConcurrencyManager::createCond(unsigned int procId){
}
void resp::ConcurrencyManager::destroyCond(unsigned int procId, int cond){
}
void resp::ConcurrencyManager::signalCond(int cond, bool broadcast, unsigned int procId){
}
int resp::ConcurrencyManager::waitCond(int cond, int mutex, double time, unsigned int procId){
}

///***************************************************************
/// Here we declare some classes which will be used as timers
///
///***************************************************************
TimeSliceClock::TimeSliceClock(const sc_time &resolution){
}
void TimeSliceClock::preempt(){
}
IRQClock::IRQClock(const sc_time &resolution){
}
void IRQClock::schedulerIRQ(){
}
EventClock::EventClock(){
}
void EventClock::schedulerEvent(){
}
CondVarClock::CondVarClock(sc_time accuracy){
}
void CondVarClock::countTime(){
}
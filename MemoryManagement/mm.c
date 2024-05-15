#include "myalloc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#define UNUSED(x) (void)(x)

struct mm_state {
    size_t blockSize;
    bool hasAlloc;
};

struct mm_module_info {
    bool nextModuleActive;
};
typedef struct mm_module_info mm_module_info_t;

size_t convertToAligned(size_t memoryAlloc) {
    return memoryAlloc + (sizeof (intmax_t)-(memoryAlloc % sizeof (intmax_t)));
}

size_t getStateSize() {
    return convertToAligned(sizeof (mm_state_t));
}

size_t getModInfoSize() {
    return convertToAligned(sizeof (mm_module_info_t));
}

void initializeModule(const struct ram_info *info, int module) {
    hw_activate(module, 0);

    mm_module_info_t* moduleInfo = (mm_module_info_t*) info->module_addrs[module];
    moduleInfo->nextModuleActive = false;

    mm_state_t* firstUnusedMemory = (mm_state_t*) (info->module_addrs[module] + getModInfoSize());
    firstUnusedMemory->hasAlloc = false;
    firstUnusedMemory->blockSize = (info->bank_size * info->nbanks_per_module) - getStateSize() - getModInfoSize();
}

mm_state_t* mm_initialize(void) {
    const struct ram_info *info = hw_raminfo();

    initializeModule(info, 0);

    return (mm_state_t*) (info->module_addrs[0] + getModInfoSize());
}

unsigned int getBankNum(const struct ram_info *info, unsigned int currentOffset) {
    unsigned int bankNum;
    for (bankNum = 0; bankNum < info->nbanks_per_module; bankNum++) {
        if (currentOffset > bankNum * info->bank_size && currentOffset <= (bankNum + 1) * info->bank_size) break;
    }
    return bankNum;
}

unsigned int getFirstUsedBank(const struct ram_info *info, unsigned int currentOffset) {
    unsigned int firstUsedBank;
    currentOffset += getStateSize();
    firstUsedBank = getBankNum(info, currentOffset);
    return firstUsedBank;
}

unsigned int getLastUsedBank(const struct ram_info *info, size_t nbytes, unsigned int currentOffset) {
    unsigned int lastUsedBank;
    currentOffset += getStateSize() + nbytes + getStateSize();
    lastUsedBank = getBankNum(info, currentOffset);
    return lastUsedBank;
}

void activateBanksIfNeeded(const struct ram_info *info, unsigned int module, mm_state_t *usedMemory, mm_state_t *prevUsedMemory, mm_state_t *nextUsedMemory, bool activate) {
    unsigned int currentOffset = (size_t) usedMemory - (size_t) info->module_addrs[module];
    unsigned int firstUsedBank = getFirstUsedBank(info, currentOffset - (prevUsedMemory != NULL && !prevUsedMemory->hasAlloc ? getStateSize() : 0)),
            lastUsedBank = getLastUsedBank(info, usedMemory->blockSize, currentOffset - (!activate && (nextUsedMemory->hasAlloc) ? getStateSize() : 0));
    
    if (prevUsedMemory != NULL && ((!prevUsedMemory->hasAlloc && getFirstUsedBank(info, (size_t) prevUsedMemory - (size_t) info->module_addrs[module] - 1) == firstUsedBank)
            || (prevUsedMemory->hasAlloc && getLastUsedBank(info, prevUsedMemory->blockSize, (size_t) prevUsedMemory - (size_t) info->module_addrs[module]) == firstUsedBank)))
        firstUsedBank++;
    if (nextUsedMemory != NULL && ((!nextUsedMemory->hasAlloc && getLastUsedBank(info, nextUsedMemory->blockSize, (size_t) nextUsedMemory - (size_t) info->module_addrs[module] - getStateSize()) == lastUsedBank && (size_t) nextUsedMemory - (size_t) info->module_addrs[module] + getStateSize() + nextUsedMemory->blockSize != info->bank_size * info->nbanks_per_module)
            || (nextUsedMemory->hasAlloc && getFirstUsedBank(info, (size_t) nextUsedMemory - (size_t) info->module_addrs[module] - getStateSize()) == lastUsedBank)))
        lastUsedBank--;

    if (lastUsedBank > info->nbanks_per_module) lastUsedBank = 0;

    for (; firstUsedBank <= lastUsedBank; firstUsedBank++) {
        if (firstUsedBank == 0) continue;
        if (activate) hw_activate(module, firstUsedBank);
        else hw_deactivate(module, firstUsedBank);
    }
}

void allocateMemoryAtCurrentOffset(size_t nbytes, const struct ram_info* info, unsigned int maxSizeInModule, unsigned int module, mm_state_t* prevUsedMemory, unsigned int currentOffset, mm_state_t* usedMemory){
    mm_state_t* nextUsedMemory = currentOffset + getStateSize() + usedMemory->blockSize >= maxSizeInModule ? NULL
            : (mm_state_t*) (info->module_addrs[module] + currentOffset + getStateSize() + usedMemory->blockSize);
    usedMemory->blockSize = nbytes;
    usedMemory->hasAlloc = true;
    activateBanksIfNeeded(info, module, usedMemory, prevUsedMemory, nextUsedMemory, true);
    mm_state_t* newUnusedBlock = (mm_state_t*) (info->module_addrs[module] + currentOffset + getStateSize() + usedMemory->blockSize);
    newUnusedBlock->hasAlloc = false;
    if (nextUsedMemory == NULL) newUnusedBlock->blockSize = maxSizeInModule - currentOffset - getStateSize() - usedMemory->blockSize - getStateSize();
    else newUnusedBlock->blockSize = (size_t) nextUsedMemory - (size_t) newUnusedBlock - getStateSize();
}

void *mm_alloc(mm_state_t* st, size_t nbytes) {
    UNUSED(st);
    nbytes = convertToAligned(nbytes);
    const struct ram_info *info = hw_raminfo();

    unsigned int maxSizeInModule = info->bank_size * info->nbanks_per_module;
    if (nbytes + getModInfoSize() + getStateSize() > maxSizeInModule) // This alloc is never gonna work
        return 0;

    for (unsigned int module = 0; module < info->nmodules; module++) { // Loop over every module
        mm_module_info_t* moduleInfo = (mm_module_info_t*) info->module_addrs[module];
        mm_state_t* prevUsedMemory = NULL;
        for (unsigned int currentOffset = getModInfoSize(); currentOffset < maxSizeInModule;) {
            mm_state_t* usedMemory = (mm_state_t*) (info->module_addrs[module] + currentOffset);
            if (!usedMemory->hasAlloc && usedMemory->blockSize >= nbytes + getStateSize()) { // Check if there is still enough space for both this alloc + a new state struct
                allocateMemoryAtCurrentOffset(nbytes, info, maxSizeInModule, module, prevUsedMemory, currentOffset, usedMemory);
                return info->module_addrs[module] + currentOffset + getStateSize();
            }
            currentOffset += usedMemory->blockSize + getStateSize();
            prevUsedMemory = usedMemory;
        }
        if (!moduleInfo->nextModuleActive && module + 1 < info->nmodules) {
            initializeModule(info, module + 1);
            moduleInfo->nextModuleActive = true;
        }
    }
    return 0;
}

void freeMemoryAtOffset(const struct ram_info* info, unsigned int module, mm_state_t* prevUsedMemory, unsigned int currentOffset, mm_state_t* usedMemory){
    mm_state_t* nextUsedMemory = (mm_state_t*) (info->module_addrs[module] + currentOffset + getStateSize() + usedMemory->blockSize);
    int newUsedMemoryBlocksize = usedMemory->blockSize;
    if (prevUsedMemory == NULL || prevUsedMemory->hasAlloc) { // Prev used is not free
        usedMemory->hasAlloc = false;
        if (!nextUsedMemory->hasAlloc)
            newUsedMemoryBlocksize += nextUsedMemory->blockSize + getStateSize();
    } else {
        prevUsedMemory->blockSize += usedMemory->blockSize + getStateSize();
        if (!nextUsedMemory->hasAlloc)
            prevUsedMemory->blockSize += nextUsedMemory->blockSize + getStateSize();
    }
    bool nextUsedMemoryHasAlloc = nextUsedMemory->hasAlloc;
    activateBanksIfNeeded(info, module, usedMemory, prevUsedMemory, nextUsedMemory, false);
    if ((prevUsedMemory == NULL || prevUsedMemory->hasAlloc) && !nextUsedMemoryHasAlloc)
        usedMemory->blockSize = newUsedMemoryBlocksize;
}

void mm_free(mm_state_t* st, void *ptr) {
    UNUSED(st);
    const struct ram_info *info = hw_raminfo();
    unsigned int maxSizeInModule = info->bank_size * info->nbanks_per_module;
    for (unsigned int module = 0; module < info->nmodules; module++) { // Find module in which this state is defined.
        mm_module_info_t* moduleInfo = (mm_module_info_t*) info->module_addrs[module];
        if (ptr > info->module_addrs[module] && ptr < info->module_addrs[module] + maxSizeInModule) {
            mm_state_t* prevUsedMemory = NULL;
            for (unsigned int currentOffset = getModInfoSize(); currentOffset < maxSizeInModule;) {
                mm_state_t* usedMemory = (mm_state_t*) (info->module_addrs[module] + currentOffset);
                if (ptr == info->module_addrs[module] + currentOffset + getStateSize()) {
                    freeMemoryAtOffset(info, module, prevUsedMemory, currentOffset, usedMemory);
                    return;
                }
                currentOffset += usedMemory->blockSize + getStateSize();
                prevUsedMemory = usedMemory;
            }
        }
        if (!moduleInfo->nextModuleActive) break;
    }
}

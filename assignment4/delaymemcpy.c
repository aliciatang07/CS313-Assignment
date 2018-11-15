#include "delaymemcpy.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include <math.h>

#define MAX_PENDING_COPIES 50

/* Data structure use to keep a list of pending memory copies. */
typedef struct pending_copy {
    
    /* Flag to indicate if this pending copy object is in use or free */
    unsigned int in_use:1;
    
    /* Source, destination and size of the memory regions involved in the copy */
    void *src;
    void *dst;
    size_t size;
    
    /* Next pending copy. NULL if there is no other pending copy */
    struct pending_copy *next;
    
} pending_copy_t;

/* First element of the pending copy linked list. The order of the
 * list matters: if two or more copies have overlapping regions, they
 * must be performed in the order of the list.
 */
static pending_copy_t *first_pending_copy = NULL;

/* Array of possible pending copies pointers. Because malloc/free are
 not async-safe they cannot safely be called inside a signal
 handler, so an array of objects is created, and the in_use flag is
 used to determine which objects are in use.
 */
static pending_copy_t pending_copy_slots[MAX_PENDING_COPIES];

/* Global variable to keep the current page size. Initialized in
 initialize_delay_memcpy_data.
 */
static long page_size = 0;

/* Returns the pointer to the start of the page that contains the
 specified memory address.
 */
static void *page_start(void *ptr) {
    
    return (void *) (((intptr_t) ptr) & -page_size);
}

/* Returns TRUE (non-zero) if the address 'ptr' is in the range of
 addresses that starts at 'start' and has 'size' bytes (i.e., ends at
 'start+size-1'). Returns FALSE (zero) otherwise.
 */
static int address_in_range(void *start, size_t size, void *ptr) {
    
    /* TO BE COMPLETED BY THE STUDENT */
    if(ptr>=start&&ptr<=start+size-1){
        return 1;
    } else{
        return 0;
    }
}

/* Returns TRUE (non-zero) if the address 'ptr' is in the same page as
 any address in the range of addresses that starts at 'start' and
 has 'size' bytes (i.e., ends at 'start+size-1'). In other words,
 returns TRUE if the address is in the range [start, start+size-1],
 or is in the same page as either start or start+size-1. Returns
 FALSE (zero) otherwise.
 */
static int address_in_page_range(void *start, size_t size, void *ptr) {
    /* TO BE COMPLETED BY THE STUDENT */
    if(address_in_range(start,size,ptr))
        return 1;
    if(ptr<page_start(start+size-1)+(size_t)page_size && ptr>=page_start(start))
        return 1;
    return 0;
}

/* mprotect requires the start address to be aligned with the page
 size. This function calls mprotect with the start of the page that
 contains ptr, and adjusts the size accordingly to include the extra
 bytes required for that adjustment.
 
 Obs: according to POSIX documentation, mprotect cannot safely be
 called inside a signal handler. However, in most modern Unix-based
 systems (including Linux), mprotect is async-safe, so it can be
 called without problems.
 */
static int mprotect_full_page(void *ptr, size_t size, int prot) {
    
    void *page = page_start(ptr);
    return mprotect(page, size + (ptr - page), prot);
}

/* Changes the permission of the pages to allow them to be copied,
 then performs the actual copy.
 */
static void actual_copy(void *dst, void *src, size_t size) {
    /* TO BE COMPLETED BY THE STUDENT */
    //change permission
    mprotect_full_page(src,size,PROT_READ|PROT_WRITE);
    mprotect_full_page(dst,size,PROT_READ|PROT_WRITE);
    //actual copy
    memcpy(dst,src,size);
}

/* Adds a pending copy object to the list of pending copies. If
 base_copy is NULL, adds the object to the end of the list,
 otherwise adds it right after base_copy.
 */
static void add_pending_copy(void *dst, void *src, size_t size, pending_copy_t *base_copy) {
    int i;
    
    // Find an available copy object slot. This would usually be done
    // with a call to malloc, but since malloc cannot safely be called
    // inside a signal handler, this is done with an array of
    // "pre-allocated" objects.
    pending_copy_t *new_copy = NULL;
    for (i = 0; i < MAX_PENDING_COPIES; i++) {
        if (!pending_copy_slots[i].in_use) {
            new_copy = &pending_copy_slots[i];
            break;
        }
    }
    
    // If there is no available copy, force the oldest copy object to be copied to make room
    if (new_copy == NULL) {
        actual_copy(first_pending_copy->dst, first_pending_copy->src, first_pending_copy->size);
        new_copy = first_pending_copy;
        first_pending_copy = new_copy->next;
    }
    
    if (!base_copy && first_pending_copy)
        for (base_copy = first_pending_copy; base_copy->next; base_copy = base_copy->next);
    
    new_copy->src = src;
    new_copy->dst = dst;
    new_copy->size = size;
    new_copy->in_use = 1;
    
    if (base_copy == new_copy) {
        // The base copy is the old slot of the current value, so add to start
        new_copy->next = first_pending_copy;
        first_pending_copy = new_copy;
    }
    if (base_copy) {
        new_copy->next = base_copy->next;
        base_copy->next = new_copy;
    }
    else {
        new_copy->next = NULL;
        first_pending_copy = new_copy;
    }
}

/* Removes a pending copy object from the list of pending copies.
 */
static void remove_pending_copy(pending_copy_t *copy) {
    
    pending_copy_t *prev = NULL;
    
    if (!copy) return;
    
    if (copy == first_pending_copy)
        first_pending_copy = copy->next;
    else {
        
        for (prev = first_pending_copy; prev && prev->next && prev->next != copy; prev = prev->next);
        if (prev)
            prev->next = copy->next;
    }
    
    copy->in_use = 0;
}

/* Returns the oldest pending copy object for which the source or
 destination range contains the provided address, either within the
 range itself, or in the same page in the page table. Returns NULL
 if no such object exists in the list.
 */
static pending_copy_t *get_pending_copy(void *ptr) {
    
    pending_copy_t *copy;
    
    for (copy = first_pending_copy; copy; copy = copy->next) {
        
        if (address_in_page_range(copy->src, copy->size, ptr) ||
            address_in_page_range(copy->dst, copy->size, ptr))
            return copy;
    }
    return NULL;
}
//add back the modified pending copy and lock this area
int addback_pending_copy(void *dst, void *src, size_t size, pending_copy_t *base_copy){
    //add the modified pending copy to pending list
    add_pending_copy(dst,src,size,base_copy);
    //lock the area
    mprotect_full_page(src, size, PROT_READ);
    mprotect_full_page(dst, size, PROT_NONE);
    return 1;
}

//cases when src,dst have different offset
void unbalanced_copy(pending_copy_t *copy,void *ptr){
    
    if(page_start(copy->src)==copy->src){
        //src at the begining of page
        void *current_src=copy->src;
        void *current_dst=copy->dst;
        void *offset_diff = (void *)(copy->dst-page_start(copy->dst));
        //locate ptr
        while(page_start(ptr)!=current_src&&!(ptr<current_dst+page_size&&ptr>=current_dst)){
            current_src+=page_size;
            current_dst+=page_size;
        }
        size_t copysize = (size_t)page_size;
        if(page_start((void *)ptr)==page_start(copy->src+copy->size-1)||page_start((void *)ptr)==page_start((void *)(copy->dst+copy->size-1-offset_diff))){
            void *end_object = copy->src+copy->size-1;
            copysize=(size_t)(end_object-page_start(end_object)+1);
        }
        actual_copy(current_dst,current_src,copysize);
        //deal with add new pending and remove old
        if(copy->size>copysize){
            if(page_start((void *)ptr)==copy->src||( ptr<current_dst+page_size&&ptr>=current_dst)){
                //first page case
                addback_pending_copy(copy->dst+(size_t)page_size,copy->src+(size_t)page_size,copy->size-(size_t)page_size,copy);
                remove_pending_copy(copy);
            }
            else if(page_start((void *)ptr)==page_start((void *)(copy->src+copy->size-1))||page_start((void *)ptr)==page_start((void *)(copy->dst+copy->size-1-offset_diff))){
                //last page case
                addback_pending_copy(copy->dst,copy->src,copy->size-copysize,copy);
                remove_pending_copy(copy);
            }
            else{
                //middle page case
                addback_pending_copy(copy->dst,copy->src,current_dst-copy->dst,copy);
                addback_pending_copy(current_dst+(size_t)page_size,current_src+(size_t)page_size,copy->size-(size_t)page_size-(size_t)(current_dst-copy->dst),copy);
                remove_pending_copy(copy);
            }
        } else{
            //directly remove the whole pending copy since the whole copy is finished
            remove_pending_copy(copy);
        }
    }
    else{
        // src, dst start not at the begining of page but with the same offset
        
        void *current_dst=copy->dst;
        void *current_src=copy->src;
        size_t copysize=0;
        
        void *offset_diff=(void *)(copy->dst-page_start(copy->dst)-(copy->src-page_start(copy->src)));
        int page_index;
        if(address_in_page_range(copy->src,copy->size,ptr)){
            page_index = (int)((ptr-copy->src)/page_size);
            
        }else{
            page_index = (int)((ptr-copy->dst)/page_size);
        }
        
        current_src=page_start(current_src+page_size*page_index);
        current_dst=current_dst+page_size*page_index;
        if(page_start(ptr)==page_start(current_src)){
            if(page_start(ptr)==page_start(copy->src+copy->size-1)){
                void *end_object = copy->src+copy->size-1;
                copysize=(size_t)(end_object-page_start(end_object)+1);
            }else{
                copysize=(size_t)page_size-(current_src-page_start(current_src)+1);
            }
        }
        else if (ptr<current_dst+page_size&&ptr>=current_dst){
            
            if(page_start((void *)ptr)==page_start((void *)(copy->dst+copy->size-1-offset_diff))){
                
                void *end_object = copy->src+copy->size-1;
                copysize=(size_t)(end_object-page_start(end_object)+1);
            }else{
                copysize=(size_t)page_size-(current_dst-page_start(current_dst)+1);
            }
        }
        actual_copy(current_dst,current_src,copysize);
        //deal with add new pending and remove old
        if(copy->size>copysize){
            if(page_start(ptr)==page_start(copy->src)||page_start(ptr)==page_start(copy->dst)){
                addback_pending_copy(page_start(copy->dst+(size_t)page_size),page_start(copy->src+(size_t)page_size),copy->size-copysize,copy);
                remove_pending_copy(copy);
            }
            //last page
            else if(page_start((void *)ptr)==page_start(copy->src+copy->size-1)||page_start((void *)ptr)==page_start((void *)(copy->dst+copy->size-1-offset_diff))){
                addback_pending_copy(copy->dst,copy->src,copy->size-copysize,copy);
                remove_pending_copy(copy);
            }
            //middle page
            else{
                addback_pending_copy(copy->dst,copy->src,current_dst-copy->dst,copy);
                addback_pending_copy(current_dst+(size_t)page_size,current_src+(size_t)page_size,copy->size-(size_t)page_size-(current_dst-copy->dst),copy);
                remove_pending_copy(copy);
            }
        }
        else{
            //directly remove the whole pending copy since the whole copy is finished
            remove_pending_copy(copy);
        }
    }
    
}



//copy helper for src,dst have the same offset
void balanced_copy(pending_copy_t *copy,void *ptr){
    mprotect_full_page(copy->src, copy->size, 2);
    mprotect_full_page(copy->dst, copy->size, 2);
    //src,dst start at the begining of page
    if(page_start(copy->src)==copy->src){
        void *current_src=copy->src;
        void *current_dst=copy->dst;
        int page_index=0;
        //find page index and locate the ptr
        if(address_in_page_range(copy->src,copy->size,ptr)){
            page_index = (int)((ptr-copy->src)/page_size);
        }else{
            page_index = (int)((ptr-copy->dst)/page_size);
        }
        current_src=copy->src+page_size*page_index;
        current_dst=copy->dst+page_size*page_index;
        size_t copysize = (size_t)page_size;
        //for last page case, copy size may less than one, calculated from offset
        if(page_start(ptr)==page_start(copy->src+copy->size-1)||page_start(ptr)==page_start(copy->dst+copy->size-1)){
            void *end_object = copy->src+copy->size-1;
            copysize=(size_t)(end_object-page_start(end_object)+1);
        }
        actual_copy(current_dst,current_src,copysize);
        mprotect_full_page(copy->src,copy->size,PROT_READ|PROT_WRITE);
        mprotect_full_page(copy->dst,copy->size,PROT_READ|PROT_WRITE);
        //if there are remaining copy in the pending copy, add modified copy and remove old copy
        if(copy->size>copysize){
            //first page case
            if(page_start(ptr)==copy->src||page_start(ptr)==copy->dst){
                addback_pending_copy(copy->dst+(size_t)page_size,copy->src+(size_t)page_size,copy->size-copysize,copy);
                remove_pending_copy(copy);
            }
            //last page case
            else if(page_start(ptr)==page_start(copy->src+copy->size-1)||page_start(ptr)==page_start(copy->dst+copy->size-1)){
                addback_pending_copy(copy->dst,copy->src,copy->size-copysize,copy);
                remove_pending_copy(copy);
            }
            //middle page case
            else{
                addback_pending_copy(current_dst+(size_t)page_size,current_src+(size_t)page_size,copy->size-(size_t)page_size-(size_t)(current_dst-copy->dst),copy);
                addback_pending_copy(copy->dst,copy->src,current_dst-copy->dst,copy);
                remove_pending_copy(copy);
            }
        } else{
            //directly remove the whole pending copy since the whole copy is finished
            remove_pending_copy(copy);
        }
    }
    else{
        // src, dst start not at the begining of page but with the same offset
        size_t copysize=0;
        void *current_dst=copy->dst;
        void *current_src=copy->src;
        int page_index;
        //find page index and locate the ptr
        if(address_in_page_range(copy->src,copy->size,ptr)){
            page_index = (int)((ptr-copy->src)/page_size);
        }else{
            page_index = (int)((ptr-copy->dst)/page_size);
        }
        
        if(page_index>0)
        {
            current_src=page_start(current_src+page_size*page_index);
            current_dst=page_start(current_dst+page_size*page_index);
        }
        //ptr at src
        if(page_start(ptr)==page_start(current_src)){
            if(page_start(ptr)==page_start(copy->src+copy->size-1)||page_start(ptr)==page_start(copy->dst+copy->size-1)){
                //for last page case, copy size may less than one, calculated from offset
                void *end_object = copy->src+copy->size-1;
                copysize=(size_t)(end_object-page_start(end_object)+1);
            }else{
                copysize=(size_t)page_size-(current_src-page_start(current_src)+1);
            }
        }
        //ptr at dst
        else if (page_start(ptr)==page_start(current_dst)){
            if(page_start(ptr)==page_start(copy->src+copy->size-1)||page_start(ptr)==page_start(copy->dst+copy->size-1)){
                //for last page case, copy size may less than one, calculated from offset
                void *end_object = copy->src+copy->size-1;
                copysize=(size_t)(end_object-page_start(end_object)+1);
            }else{
                copysize=(size_t)page_size-(current_dst-page_start(current_dst)+1);
            }
        }
        actual_copy(current_dst,current_src,copysize);
        mprotect_full_page(copy->src,copy->size,PROT_READ|PROT_WRITE);
        mprotect_full_page(copy->dst,copy->size,PROT_READ|PROT_WRITE);
        //deal with add new pending and remove old
        if(copy->size>copysize){
            //fist page case
            if(page_start(ptr)==page_start(copy->src)||page_start(ptr)==page_start(copy->dst)){
                addback_pending_copy(page_start(copy->dst+page_size-1),page_start(copy->src+page_size-1),copy->size-copysize,copy);
                remove_pending_copy(copy);
            }
            //last page case
            else if(page_start(ptr)==page_start(copy->src+copy->size-1)||page_start(ptr)==page_start(copy->dst+copy->size-1)){
                addback_pending_copy(copy->dst,copy->src,copy->size-copysize,copy);
                remove_pending_copy(copy);
            }
            //middle page case
            else{
                addback_pending_copy(current_dst+(size_t)page_size,current_src+(size_t)page_size,copy->size-(size_t)page_size-(size_t)(current_dst-copy->dst),copy);
                addback_pending_copy(copy->dst,copy->src,current_dst-copy->dst,copy);
                remove_pending_copy(copy);
            }
        }
        else{
            //directly remove the whole pending copy since the whole copy is finished
            remove_pending_copy(copy);
        }
    }
    
}


//helper involved with handling actual copy regarding requested address
void copyhelper(pending_copy_t *copy,void *ptr){
    if(copy->dst-page_start(copy->dst)==copy->src-page_start(copy->src)){
        balanced_copy(copy,(void *)ptr);
    }else{
        //case when src,dst have different offset
        unbalanced_copy(copy,(void *)ptr);
    }
    
}

//recursion for dependecy case
void recursion_checker(pending_copy_t *copy,void *ptr){
    pending_copy_t *copyo;
    //A->B B->C dependency
    for(copyo=first_pending_copy;copyo<copy;copyo=copyo->next){
        if (address_in_page_range(copyo->dst, copyo->size, copy->src)){
            recursion_checker(copyo, copyo->src);
        }
    }

    copyhelper(copy,ptr);
    
    //A->B C->A dependency
    for(copyo=copy->next;copyo;copyo=copyo->next){
        if (address_in_page_range(copyo->src, copyo->size, ptr)){

            recursion_checker(copyo, copyo->src);
            
        }
    }
    //A->B C->B dependency
            for(copyo=copy->next;copyo;copyo=copyo->next){
                if (address_in_page_range(copyo->dst, copyo->size, ptr))
                {
                    recursion_checker(copyo,copyo->dst);
                }
            }
}




/* Segmentation fault handler. If the address that caused the
 segmentation fault (represented by info->si_addr) is part of a
 pending copy, this function will perform the copy for the entire
 page that contains the address. If the pending copy corresponding
 to the address involves more than one page, the object will be
 replaced with the remaining pages (before and after the affected
 page), otherwise it will be removed from the list. If the address
 is not part of a pending copy page, the process will write a
 message to the standard error (stderr) output and kill the process.
 */
static void delay_memcpy_segv_handler(int signum, siginfo_t *info, void *context) {
    pending_copy_t *copy = get_pending_copy(info->si_addr);
//    pending_copy_t *copyo;
    void *ptr = info->si_addr; // requested address
    if (copy == NULL) {
        write(STDERR_FILENO, "Segmentation fault!\n", 20);
        raise(SIGKILL);
    }
    /* TO BE COMPLETED BY THE STUDENT */
    else{
        //check dependency before actual copy
        recursion_checker(copy,ptr);

    }

}


/* Initializes the data structures and global variables used in the
 delay memcpy process. Changes the signal handler for segmentation
 fault to the handler used by this process.
 */
void initialize_delay_memcpy_data(void) {
    
    struct sigaction sa;
    sa.sa_sigaction = delay_memcpy_segv_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);
    
    page_size = sysconf(_SC_PAGESIZE);
}

/* Starts the copying process of 'size' bytes from 'src' to 'dst'. The
 actual copy of data is performed in the signal handler for
 segmentation fault. This function only stores the information
 related to the copy in the internal data structure, and protects
 the pages (source as read-only, destination as no access) so that
 the signal handler is invoked when the copied data is needed. If
 the maximum number of pending copies is reached, a copy is
 performed immediately. Returns the value of dst.
 */
void *delay_memcpy(void *dst, void *src, size_t size) {
    //add the copy to pending list
    add_pending_copy(dst,src,size,NULL);
    //lock the area so access to dst or write to region
    mprotect_full_page(src, size, PROT_READ);
    mprotect_full_page(dst, size, PROT_NONE);
    return dst;
}


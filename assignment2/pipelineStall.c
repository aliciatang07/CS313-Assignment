#include "pipeline.h"


/* Pipeline control unit. This implementation uses only stalls when
 required to avoid data and control hazards. */
int pipelineControl(const int_reg_fd_t *f, const int_reg_de_t *d, const int_reg_em_t *e,
                    const int_reg_mw_t *m, const int_reg_ret_t *w,
                    int_reg_fetch_t *F, int_reg_fd_t *D, int_reg_de_t *E, int_reg_em_t *M,
                    int_reg_mw_t *W) {
    
    /* TO BE COMPLETED BY THE STUDENT */
    
    // check control hazard and stall
    
    //JXX
    if(f->iCode == 7 && f->iFun !=0 && F->status!= ST_STALLED){
        F->status = ST_STALLED;
        F->PC = f->nextPC;
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    if(d->iCode == 7 && d->iFun !=0 && D->status!= ST_BUBBLE){
        F->status = ST_STALLED;
        F->PC = f->nextPC;
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        D->status = ST_BUBBLE;
        D->iCode = 1;
        D->iFun = 0;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    if(e->iCode == 7 && e->iFun !=0 && E->status!= ST_BUBBLE){
        if (e->bch == 1) {
            F->PC = E->valC;
        } else {
            F->PC = E->valA;
        }
        F->status = ST_REGULAR;
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        D->status = ST_BUBBLE;
        D->iCode = 1;
        D->iFun = 0;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    
    //RET
    if(m->iCode == 9 && m->iFun == 0 && M->status != ST_BUBBLE){
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        F-> PC = m ->valM;
        F-> status = ST_REGULAR;
        D-> status = ST_BUBBLE;
        D->iCode = 1;
        D->iFun = 0;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    if(e->iCode ==9 ){
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        F->status = ST_STALLED;
        D->status = ST_BUBBLE;
        D->iCode = 1;
        D->iFun = 0;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    if(d->iCode == 9 ){
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        F->status = ST_STALLED;
        F->PC = f->nextPC;
        
        D->status = ST_BUBBLE;
        D->iCode = 1;
        D->iFun = 0;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    if(f->iCode == 9){
        F->status = ST_STALLED;
        F->PC = f->nextPC;
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
        
    }
    
    
    //data hazard
    
    //key consideration when to quit and stop stalling
    
    //    //between decode and write-back
    if(m->status == ST_BUBBLE && D->status == ST_STALLED){
        F->PC = f->nextPC;
        F->status = ST_REGULAR;
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        E->status = ST_BUBBLE;
        D->status = ST_REGULAR;
        E->iCode = 1;
        E->iFun = 0;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    
    if((f->rA!=R_NONE&&(f->rA == m->dstE||f->rA == m->dstM))||(f->iCode == I_OPQ&&f->rB!=R_NONE&&(f->rB == m->dstE||f->rB == m->dstM))||((f->iCode == I_POPQ||f->iCode==I_PUSHQ||f->iCode == I_RET||f->iCode ==I_CALL) && m->dstE ==R_RSP)) {
        //STALL
        //STALL BUBBLE
        if(m->iCode == 2 && m->iFun!=0 && m->bch ==0){
            //don't move, no data hazard
            F->status = ST_REGULAR;
            *D = *f;
            *E = *d;
            *M = *e;
            *W = *m;
            D->status = ST_REGULAR;
            E->status = ST_REGULAR;
            M->status = ST_REGULAR;
            return w->status != ST_EXCEPTION && w->iCode != I_HALT;
        }
        if(D->status!=ST_STALLED){
            F->status = ST_REGULAR;
            *D = *f;
            *E = *d;
            *M = *e;
            *W = *m;
            D->status = ST_STALLED;
            return w->status != ST_EXCEPTION && w->iCode != I_HALT;
        }
        else if(e->status == ST_BUBBLE){
            F->PC = f->nextPC;
            F->status = ST_REGULAR;
            *D = *f;
            *E = *d;
            *M = *e;
            *W = *m;
            E->status = ST_BUBBLE;
            D->status = ST_REGULAR;
            E->iCode = 1;
            E->iFun = 0;
            return w->status != ST_EXCEPTION && w->iCode != I_HALT;
        }
        else{
            // F->PC = f->nextPC;
            F->status = ST_STALLED;
            F->PC = f->nextPC;
            *D = *f;
            *E = *d;
            *M = *e;
            *W = *m;
            E->status = ST_BUBBLE;
            D->status = ST_STALLED;
            E->iCode = 1;
            E->iFun = 0;
            return w->status != ST_EXCEPTION && w->iCode != I_HALT;
        }
    }
    
    //between decode and memory
    if((f->rA!=R_NONE&&(f->rA == e->dstE||f->rA == e->dstM))||(f->iCode == I_OPQ&&f->rB!=R_NONE&&(f->rB == e->dstE||f->rB == e->dstM))||((f->iCode == I_POPQ||f->iCode==I_PUSHQ||f->iCode == I_RET||f->iCode ==I_CALL) && e->dstE ==R_RSP)) {
        if(e->iCode == 2 && e->iFun!=0 && e->bch ==0){
            //don't move, no data hazard
            F->status = ST_REGULAR;
            F->PC = f->nextPC;
            *D = *f;
            *E = *d;
            *M = *e;
            *W = *m;
            D->status = ST_REGULAR;
            E->status = ST_REGULAR;
            return w->status != ST_EXCEPTION && w->iCode != I_HALT;
        }
        if(D->status!=ST_STALLED){
            F->status = ST_STALLED;
            F->PC = f->nextPC;
            *D = *f;
            *E = *d;
            *M = *e;
            *W = *m;
            D->status = ST_STALLED;
            //F->status = ST_STALLED;
            return w->status != ST_EXCEPTION && w->iCode != I_HALT;
        }
        else{
            if(d->status == ST_BUBBLE){
                F->PC = f->nextPC;
                F->status = ST_REGULAR;
                *D = *f;
                *E = *d;
                *M = *e;
                *W = *m;
                E->status = ST_BUBBLE;
                D->status = ST_REGULAR;
                E->iCode = 1;
                E->iFun = 0;
                return w->status != ST_EXCEPTION && w->iCode != I_HALT;
            }
            //F->PC = f->nextPC;
            //F->status = ST_STALLED;
            *E = *d;
            *M = *e;
            *W = *m;
            E->status = ST_BUBBLE;
            D->status = ST_STALLED;
            E->iCode = 1;
            E->iFun = 0;
            return w->status != ST_EXCEPTION && w->iCode != I_HALT;
        }
    }
    
    // between decode and execute
    
    if((f->rA!=R_NONE&&(f->rA == d->dstE||f->rA == d->dstM))||(f->iCode == I_OPQ&&f->rB!=R_NONE&&(f->rB == d->dstE||f->rB == d->dstM))||((f->iCode == I_POPQ||f->iCode==I_PUSHQ||f->iCode == I_RET||f->iCode ==I_CALL) && d->dstE ==R_RSP)) {
        F->status = ST_STALLED;
        F->PC = f->nextPC;
        *D = *f;
        *E = *d;
        *M = *e;
        *W = *m;
        D->status = ST_STALLED;
        return w->status != ST_EXCEPTION && w->iCode != I_HALT;
    }
    
    
    F->PC = f->nextPC;
    F->status = ST_REGULAR;
    *D = *f;
    *E = *d;
    *M = *e;
    *W = *m;
    return w->status != ST_EXCEPTION && w->iCode != I_HALT;
}

